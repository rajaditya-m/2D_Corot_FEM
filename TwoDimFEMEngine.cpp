#include "TwoDimFEMEngine.h"


TwoDimFEMEngine::TwoDimFEMEngine(TwoDimMesh* o,CollisionPlane* co)
{
	object_ = o;
	collObj_ = co;

	int numPoints = object_->numPoints();

	//Resize the matrices correctly 
	M = Eigen::SparseMatrix<double>(numPoints*2,numPoints*2);

	//Assemble the mass matrix correctly
	for(int i=0;i<numPoints;i++)
	{
		M.insert(2*i,2*i) = object_->getMass(i);
		M.insert(2*i+1,2*i+1) = object_->getMass(i);
	}

	//Get the rest position buffer
	std::vector<Eigen::Vector2d> restPos = object_->getPositionVector(0);
	x0 = flattenToColumnVector(restPos);

}

TwoDimFEMEngine::~TwoDimFEMEngine(void)
{
}

void TwoDimFEMEngine::resolveCollision()
{
	int curFrame = object_->numFrames()-1;
	int numPoints = object_->numPoints();
	int numTris = object_->numTriangles();
	double coeffFric = object_->getParams()->getFriction();
	for(int p=0;p<numPoints;p++)
	{
		Eigen::Vector2d pos = localPositionBuffer_[p];
		double d = (pos.x()*collObj_->a())+(pos.y()*collObj_->b())+collObj_->c();
		if(d<0.0)
		{
			Eigen::Vector2d planePush(collObj_->a(),collObj_->b());
			localPositionBuffer_[p] -= (d*planePush);
			
			double relativeVelocity = planePush.dot(localVelocityBuffer_[p]);
			if(relativeVelocity<0.0)
			{
				Eigen::Vector2d newVelocity = -(relativeVelocity*planePush);
				newVelocity = (newVelocity + localVelocityBuffer_[p]);
				localVelocityBuffer_[p] = newVelocity;

			}
		}
	}
}

void TwoDimFEMEngine::clearLocalBuffers()
{
	forces_.clear();
	localPositionBuffer_.clear();
	localVelocityBuffer_.clear();
	newPositionBuffer_.clear();
	newVelocityBuffer_.clear();
	rotationMatrix_.clear();
}

void TwoDimFEMEngine::copyToLocalBuffers()
{
	int curFrame = object_->numFrames()-1;
	int numPoints = object_->numPoints();
	localPositionBuffer_ = object_->getPositionVector(curFrame);
	localVelocityBuffer_ = object_->getVelocityVector(curFrame);
	forces_.resize(numPoints,Eigen::Vector2d::Zero());
	newPositionBuffer_.resize(numPoints,Eigen::Vector2d::Zero());
	newVelocityBuffer_.resize(numPoints,Eigen::Vector2d::Zero());
	rotationMatrix_.resize(object_->numTriangles(),Eigen::Matrix2d::Zero());
}

void TwoDimFEMEngine::gravityForces()
{
	int numPoints = object_->numPoints();
	for(int p=0;p<numPoints;p++)
	{
		Eigen::Vector2d gravityForce(0.0,-(object_->getMass(p)*9.8));
		forces_[p] += gravityForce;
	}
}

//Only relevant in case of explicit integrator 
void TwoDimFEMEngine::shearForces()
{
	int numTris = object_->numTriangles();
	double lambda = object_->getParams()->getLameLambda();
	double mu = object_->getParams()->getLameMu();
	double damp = object_->getParams()->getLameDampness();
	for(int t=0;t<numTris;t++)
	{
		Triangles tri = object_->getTriangle(t);
		
		//Compute the matrix Ds
		Eigen::Vector2d vba = localPositionBuffer_[tri.b] - localPositionBuffer_[tri.a];
		Eigen::Vector2d vca = localPositionBuffer_[tri.c] - localPositionBuffer_[tri.a];
		Eigen::Matrix2d Ds;
		Ds(0,0) = vba(0); Ds(0,1) = vca(0);
		Ds(1,0) = vba(1); Ds(1,1) = vca(1);

		//Compute the deformation gradient 
		Eigen::Matrix2d F = Ds * object_->getPreComputedDm(t);

		//Compute the rotation matrix by polar decomposition 
		Eigen::Matrix2d R = performQRDecomposition(F);
		Eigen::Matrix2d S = R.transpose()*F;

		//Compute the small strain 
		Eigen::Matrix2d smallStrain = (0.5*(S + S.transpose())) - Eigen::Matrix2d::Identity();

		//Compute the stress tensor
		Eigen::Matrix2d stressTensor = (lambda*smallStrain.trace())*Eigen::Matrix2d::Identity() + mu*2.0f*smallStrain;
		
		//Compute the matrix Ds
		Eigen::Vector2d vvba = localVelocityBuffer_[tri.b] - localVelocityBuffer_[tri.a];
		Eigen::Vector2d vvca = localVelocityBuffer_[tri.c] - localVelocityBuffer_[tri.a];
		Eigen::Matrix2d Dvs;
		Dvs(0,0) = vvba(0); Dvs(0,1) = vvca(0);
		Dvs(1,0) = vvba(1); Dvs(1,1) = vvca(1);

		//Compute the deformation gradient 
		Eigen::Matrix2d dFdt = Dvs * object_->getPreComputedDm(t);
		Eigen::Matrix2d dSdt = R.transpose()*dFdt;
		//Compute the derivative of strain tensor 
		Eigen::Matrix2d dedt = (0.5*(dSdt + dSdt.transpose()));

		//Compute the damping tensor
		Eigen::Matrix2d dampTensor = (damp*dedt.trace())*Eigen::Matrix2d::Identity() + damp*2.0f*dedt;
		
		Eigen::Matrix2d forceTensor = stressTensor + dampTensor;
		//if(t==100)
		//	std::cout << "F :" << F << "\n";

		//Distribute the forces into their coordinates 
		Eigen::Vector2d f1 = R*forceTensor*object_->getAreaNormals(3*t+0);
		Eigen::Vector2d	f2 = R*forceTensor*object_->getAreaNormals(3*t+1);
		Eigen::Vector2d	f3 = R*forceTensor*object_->getAreaNormals(3*t+2);
		

		forces_[tri.a] -= f1/3.0f;
		forces_[tri.b] -= f2/3.0f;
		forces_[tri.c] -= f3/3.0f;

	}
}

void TwoDimFEMEngine::dampingForces()
{
	int numTris = object_->numTriangles();
	double lambda = 10000.0;
	double mu = 10000.0;
	for(int t=0;t<numTris;t++)
	{
		Triangles tri = object_->getTriangle(t);
		
		//Compute the matrix Ds
		Eigen::Vector2d vba = localVelocityBuffer_[tri.b] - localVelocityBuffer_[tri.a];
		Eigen::Vector2d vca = localVelocityBuffer_[tri.c] - localVelocityBuffer_[tri.a];
		Eigen::Matrix2d Ds;
		Ds(0,0) = vba(0); Ds(0,1) = vca(0);
		Ds(1,0) = vba(1); Ds(1,1) = vca(1);

		//Compute the deformation gradient 
		Eigen::Matrix2d F = Ds * object_->getPreComputedDm(t);

		//Compute the rotation matrix by polar decomposition 
		Eigen::Matrix2d R = performQRDecomposition(F);
		Eigen::Matrix2d S = R.transpose()*F;

		//Compute the small strain 
		Eigen::Matrix2d smallStrain = (0.5*(S + S.transpose())) - Eigen::Matrix2d::Identity();

		//Compute the stress tensor
		Eigen::Matrix2d stressTensor = (lambda*smallStrain.trace())*Eigen::Matrix2d::Identity() + mu*2.0f*smallStrain;
		//if(t==100)
		//	std::cout << "F :" << F << "\n";

		//Distribute the forces into their coordinates 
		Eigen::Vector2d f1 = R*stressTensor*object_->getAreaNormals(3*t+0);
		Eigen::Vector2d	f2 = R*stressTensor*object_->getAreaNormals(3*t+1);
		Eigen::Vector2d	f3 = R*stressTensor*object_->getAreaNormals(3*t+2);
		

		forces_[tri.a] -= f1/3.0f;
		forces_[tri.b] -= f2/3.0f;
		forces_[tri.c] -= f3/3.0f;

	}
}

void TwoDimFEMEngine::aggregateForces()
{
	shearForces();
	gravityForces();
}

void TwoDimFEMEngine::performExplicitEulerIntegration()
{
	int numPoints = object_->numPoints();
	float timestep = object_->getParams()->get_timestep();
	for(int p=0;p<numPoints;p++)
	{
		Eigen::Vector2d delV = (forces_[p]/object_->getMass(p))*timestep;
		Eigen::Vector2d newV = localVelocityBuffer_[p] + delV;
		//std::cout << newV.transpose() << "\n";
		Eigen::Vector2d newP = localPositionBuffer_[p] + (newV*timestep);
		newPositionBuffer_[p] = newP;
		newVelocityBuffer_[p] = newV;
	}
}

void TwoDimFEMEngine::updateObject()
{
	object_->appendNewFramePosition(newPositionBuffer_);
	object_->appendNewFrameVelocity(newVelocityBuffer_);	
}

void TwoDimFEMEngine::computeRotationMatrices()
{
	int numTris = object_->numTriangles();
	for(int t=0;t<numTris;t++)
	{
		Triangles tri = object_->getTriangle(t);
		
		//Compute the matrix Ds
		Eigen::Vector2d vba = localPositionBuffer_[tri.b] - localPositionBuffer_[tri.a];
		Eigen::Vector2d vca = localPositionBuffer_[tri.c] - localPositionBuffer_[tri.a];
		Eigen::Matrix2d Ds;
		Ds(0,0) = vba(0); Ds(0,1) = vca(0);
		Ds(1,0) = vba(1); Ds(1,1) = vca(1);

		//Compute the deformation gradient 
		Eigen::Matrix2d F = Ds * object_->getPreComputedDm(t);

		//Compute the rotation matrix by polar decomposition 
		Eigen::Matrix2d R = performQRDecomposition(F);

		//Assing this matrix to the rotation matrix vector
		rotationMatrix_[t] = R;
	}
}

void TwoDimFEMEngine::assembleStiffnessMatrix()
{
	//Populate the sparse stiffness matrix data
	int numEntries = object_->getSizeOfColumnVector();
	stiffnessMatrixData_.assign(numEntries,Eigen::Matrix2d::Zero());
	int numTris = object_->numTriangles();
	for(int t=0;t<numTris;t++)
	{
		Triangles triObj = object_->getTriangle(t);

		//Now we shall assemble the stiffness matrix data
		Eigen::Matrix2d R = rotationMatrix_[t];
		Eigen::Matrix2d RT = R.transpose();

		Eigen::Matrix2d entry;

		//00 
		entry = R * object_->getRestStiffnessMatrix(t*6+0) * RT;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[0],triObj[0])] += entry;

		//01 (and 10) too while we are at it 
		entry = R * object_->getRestStiffnessMatrix(t*6+1) * RT;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[0],triObj[1])] += entry;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[1],triObj[0])] += entry.transpose();

		//02 (and 20)
		entry = R * object_->getRestStiffnessMatrix(t*6+2) * RT;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[0],triObj[2])] += entry;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[2],triObj[0])] += entry.transpose();

		//11
		entry = R * object_->getRestStiffnessMatrix(t*6+3) * RT;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[1],triObj[1])] += entry;
		
		//12 (and 21)
		entry = R * object_->getRestStiffnessMatrix(t*6+4) * RT;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[1],triObj[2])] += entry;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[2],triObj[1])] += entry.transpose();

		//22
		entry = R * object_->getRestStiffnessMatrix(t*6+5) * RT;
		stiffnessMatrixData_[object_->getLocationFromFront(triObj[2],triObj[2])] += entry;

	}
}

Eigen::SparseMatrix<double> TwoDimFEMEngine::generateStiffnessMatrix()
{
	int numPoints = object_->numPoints();
	std::vector<int> columns = object_->getColumnVector();
	std::vector<int> offsets = object_->getOffsetVector();
	Eigen::SparseMatrix<double> K(numPoints*2,numPoints*2);
	for (int row = 0;row<numPoints;row++)
	{
		for(int cidx = offsets[row];cidx<offsets[row+1];cidx++)
		{
			int col = columns[cidx];
			//std::cout << "cidx:" << cidx << " cols:" << col << "\n";
			Eigen::Matrix2d mat = stiffnessMatrixData_[cidx];
			K.coeffRef(2*row+0,2*col+0) += mat(0,0);	K.coeffRef(2*row+0,2*col+1) += mat(0,1);  
			K.coeffRef(2*row+1,2*col+0) += mat(1,0);	K.coeffRef(2*row+1,2*col+1) += mat(1,1);
		}
	}
	return K;
}

void TwoDimFEMEngine::performForwardEulerIntegration()
{
	int numPoints = object_->numPoints();
	int numTris = object_->numTriangles();

	//Get the matrix K
	Eigen::SparseMatrix<double> K = generateStiffnessMatrix();

	double alpha = 0.008;
	double beta = 0.008;
	double timestep = object_->getParams()->get_timestep();
	double mFactor = (1+alpha*timestep);
	double kFactor = (beta*timestep + timestep*timestep);

	Eigen::SparseMatrix<double> A = mFactor*M + kFactor*K;

	Eigen::VectorXd fext = flattenToColumnVector(forces_);
	Eigen::VectorXd vn = flattenToColumnVector(localVelocityBuffer_);
	Eigen::VectorXd xn = flattenToColumnVector(localPositionBuffer_);
	Eigen::VectorXd B = (timestep*fext) + (M*vn) - (timestep*K*(xn-x0));
	//Now solve using conjugate gradient methods
	Eigen::VectorXd x(2*numPoints);
	Eigen::ConjugateGradient<Eigen::SparseMatrix<double> > cg;
	cg.compute(A);
	x = cg.solve(B);
	//std::cout << "ERROR: " << cg.error() << " ITERATIONS: " <<  cg.iterations() << "\n";

	//Transfer to the new buffers 
	for(int p=0;p<numPoints;p++)
	{
		Eigen::Vector2d newV(x(2*p),x(2*p+1));
		newVelocityBuffer_[p] = newV;
		newPositionBuffer_[p] = localPositionBuffer_[p] + newV*timestep;
	}

}

void TwoDimFEMEngine::generateNextFrame()
{
	bool explicitSolver = false;
	//Common to both solvers
	clearLocalBuffers();
	copyToLocalBuffers();
	resolveCollision();

	if(explicitSolver)
	{	
		aggregateForces();
		performExplicitEulerIntegration();
	}
	else
	{
		gravityForces();
		computeRotationMatrices();
		assembleStiffnessMatrix();
		performForwardEulerIntegration();
	}
	updateObject();
}