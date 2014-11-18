#include "TwoDimMesh.h"



TwoDimMesh::TwoDimMesh(const char* initOBJ,const char* paramFile)
{
	params_ = new PhysicalParameters(paramFile);
	 
	std::ifstream fileIn(initOBJ);
	std::string line;
	std::string first;
	double x,y,z;
	int a,b,c;
	std::stringstream ss;
	std::vector<Eigen::Vector2d> firstFramePoints;
	while(std::getline(fileIn,line))
	{
		ss.clear();
		ss.str("");
		ss << line;
		ss >> first;
		if(!first.compare("v"))
		{
			ss >> x >> y >> z;
			Eigen::Vector2d temp(x,y);
			firstFramePoints.push_back(temp);
		}
		else if(!first.compare("f"))
		{
			ss >> a >> b >> c;
			Triangles t(a-1,b-1,c-1);
			triangles_.push_back(t);
		}
	}
	position_.push_back(firstFramePoints);
	std::cout << "[INFO] Number of points read : " << firstFramePoints.size() << "\n";
	std::cout << "[INFO] Number of triangles read : " << triangles_.size() << "\n";
	
	//Initialize the velcity vector 
	std::vector<Eigen::Vector2d> vel0(firstFramePoints.size(),Eigen::Vector2d::Zero());
	velocity_.push_back(vel0);

	//@TODO: Initialize the mass vector
	mass_.resize(numPoints(),0.0);
	area_.resize(numTriangles(),0.0);
	for(int t=0;t<numTriangles();t++)
	{
		Eigen::Vector2d vba = position_[0][triangles_[t].b] - position_[0][triangles_[t].a];
		Eigen::Vector2d vca = position_[0][triangles_[t].c] - position_[0][triangles_[t].a];
		double mag = fabs((vba.x()*vca.y())-(vca.x()*vba.y()));
		mag *= 0.5;
		area_[t] = mag;
		double massPerTri = mag*(getParams()->getDensity());
		mass_[triangles_[t].a] += massPerTri*0.33;
		mass_[triangles_[t].b] += massPerTri*0.33;
		mass_[triangles_[t].c] += massPerTri*0.33;
	}

	//Now compute the value of matrix B for each triangle which will be required 
	areaWeighedNormals_.resize(3*numTriangles(),Eigen::Vector2d::Zero());
	for(int t=0;t<numTriangles();t++)
	{
		Eigen::Matrix2d m;
		Eigen::Vector2d vba = position_[0][triangles_[t].b] - position_[0][triangles_[t].a];
		Eigen::Vector2d vca = position_[0][triangles_[t].c] - position_[0][triangles_[t].a];
		Eigen::Vector2d vcb = position_[0][triangles_[t].c] - position_[0][triangles_[t].b];
		m(0,0) = vba(0); m(0,1) = vca(0);
		m(1,0) = vba(1); m(1,1) = vca(1);
		Eigen::Matrix2d invM = m.inverse().eval();
		Dm.push_back(invM);
		areaWeighedNormals_[3*t+0] = Eigen::Vector2d(-vcb.y(),vcb.x());
		areaWeighedNormals_[3*t+1] = Eigen::Vector2d(vca.y(),-vca.x());
		areaWeighedNormals_[3*t+2] = Eigen::Vector2d(-vba.y(),vba.x());
	}

	//Now we compute the rest stiffness matrix for each of the elements
	restStiffnessMatrix_.resize(6*numTriangles(),Eigen::Matrix2d::Zero());
	forceOffsets_.resize(3*numTriangles(),Eigen::Vector2d::Zero());
	double lambda = getParams()->getLameLambda();
	double mu = getParams()->getLameMu();
	for(int t=0;t<numTriangles();t++)
	{
		Eigen::Matrix2d outerPdk[6] = { areaWeighedNormals_[3*t+0]*areaWeighedNormals_[3*t+0].transpose(),  //00
										areaWeighedNormals_[3*t+0]*areaWeighedNormals_[3*t+1].transpose(),  //01
										areaWeighedNormals_[3*t+0]*areaWeighedNormals_[3*t+2].transpose(),  //02
										areaWeighedNormals_[3*t+1]*areaWeighedNormals_[3*t+1].transpose(),  //11
										areaWeighedNormals_[3*t+1]*areaWeighedNormals_[3*t+2].transpose(),  //12
										areaWeighedNormals_[3*t+2]*areaWeighedNormals_[3*t+2].transpose() };  //22
		
		double innerPdk[6] = {	areaWeighedNormals_[3*t+0].dot(areaWeighedNormals_[3*t+0]),  //00
								areaWeighedNormals_[3*t+0].dot(areaWeighedNormals_[3*t+1]),  //01
								areaWeighedNormals_[3*t+0].dot(areaWeighedNormals_[3*t+2]),  //02
								areaWeighedNormals_[3*t+1].dot(areaWeighedNormals_[3*t+1]),  //11
								areaWeighedNormals_[3*t+1].dot(areaWeighedNormals_[3*t+2]),  //12
								areaWeighedNormals_[3*t+2].dot(areaWeighedNormals_[3*t+2]) };  //22

		for(int i=0;i<6;i++)
		{
			restStiffnessMatrix_[t*6+i] =  (1.0/area_[t])*lambda*outerPdk[i] + mu*(innerPdk[i]*Eigen::Matrix2d::Identity() + outerPdk[i].transpose());
			//std::cout << restStiffnessMatrix_[t*6+i] << "\n";
		}

		forceOffsets_[3*t+0] =	restStiffnessMatrix_[t*6+0]*position_[0][triangles_[t].a] + 
								restStiffnessMatrix_[t*6+1]*position_[0][triangles_[t].b] +
								restStiffnessMatrix_[t*6+2]*position_[0][triangles_[t].c] ;

		forceOffsets_[3*t+1] =	restStiffnessMatrix_[t*6+1].transpose()*position_[0][triangles_[t].a] + 
								restStiffnessMatrix_[t*6+3]*position_[0][triangles_[t].b] +
								restStiffnessMatrix_[t*6+4]*position_[0][triangles_[t].c] ;

		forceOffsets_[3*t+2] =	restStiffnessMatrix_[t*6+2].transpose()*position_[0][triangles_[t].a] + 
								restStiffnessMatrix_[t*6+4].transpose()*position_[0][triangles_[t].b] +
								restStiffnessMatrix_[t*6+5]*position_[0][triangles_[t].c] ;
	}

	//Now we determine the adjacency list 
	std::vector<std::vector<int> > neighbourList(numPoints());
	for(int t=0;t<numTriangles();t++)
	{
		//int nodeArray[3] = {triangles_[t].a,triangles_[t].b,triangles_[t].c};
		for(int i=0;i<3;i++)
		{
			neighbourList[triangles_[t][i]].push_back(triangles_[t][i]);
			for(int j=0;j<3;j++)
			{
				if(triangles_[t][i]<triangles_[t][j])
				{
					neighbourList[triangles_[t][i]].push_back(triangles_[t][j]);
					neighbourList[triangles_[t][j]].push_back(triangles_[t][i]);
				}
			}
		}
	}

	//Duplicate clean up because it looks friggin ugly 
	for(int i=0;i<neighbourList.size();i++)
	{
		std::sort( neighbourList[i].begin(), neighbourList[i].end() );
		neighbourList[i].erase( std::unique( neighbourList[i].begin(), neighbourList[i].end() ), neighbourList[i].end() );
	}

	int colsize = 0;
	for(int i=0;i<numPoints();i++)
	{
		colsize +=  neighbourList[i].size();
	}
	columns_.resize(colsize);
	offsets_.resize(numPoints()+1);
	offsets_[0] = 0;
	for(int i=0;i<numPoints();i++)
	{
		offsets_[i+1] = offsets_[i] + neighbourList[i].size();
		std::copy(neighbourList[i].begin(), neighbourList[i].end(),columns_.begin() + offsets_[i]);
	}  
}


TwoDimMesh::~TwoDimMesh(void)
{
}

int TwoDimMesh::getLocationFromFront(int row, int col)
{
	std::vector<int>::iterator it = std::find(columns_.begin() + offsets_[row], columns_.begin() + offsets_[row+1], col);
	if(*it!=col)
	{
		std::cout << "[ERROR] All hell hath broken loose!\n";
		exit(1);
	}
	return std::distance(columns_.begin(),it);

}

void TwoDimMesh :: render(int frameId)
{
	if (frameId >= numFrames())
	{
		std::cout << "[ERROR] You are trying to render a frame not yet generated.\n";
		return ;
	}
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f,1.0f,1.0f);
	//This function draws the triangles
	glBegin(GL_TRIANGLES);
	for(int t=0;t<this->numTriangles();t++)
	{
		Eigen::Vector2d p1 = position_[frameId][triangles_[t].a];
		Eigen::Vector2d p2 = position_[frameId][triangles_[t].b];
		Eigen::Vector2d p3 = position_[frameId][triangles_[t].c];

		glVertex3f(p1.x(),p1.y(),0.0);
		glVertex3f(p2.x(),p2.y(),0.0);
		glVertex3f(p3.x(),p3.y(),0.0);
	}
	glEnd();
	//This draws the points 
	glColor3f(1.0,1.0,1.0);
	glPointSize(8.0);
	glBegin(GL_POINTS);
	for(int j=0;j<numPoints();j++)
	{
		glVertex3d(position_[frameId][j].x(),position_[frameId][j].y(),0.0);
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}
