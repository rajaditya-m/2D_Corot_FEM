#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "TwoDimMesh.h"
#include "CollisionPlane.h"
#include "Matrix2DMaths.h"

class TwoDimFEMEngine
{
public:
	TwoDimFEMEngine(TwoDimMesh* o,CollisionPlane* co);
	~TwoDimFEMEngine(void);

	//This is the main method which is called outside
	void generateNextFrame();

	//Resolve brute force collisions 
	void resolveCollision();

	//Find the components of each forces mostly for the explicit solvers
	void aggregateForces();
	void gravityForces();
	void shearForces();
	void dampingForces();

	//These are for the implicit solvers
	void computeRotationMatrices();
	void assembleStiffnessMatrix();
	void performForwardEulerIntegration();
	Eigen::SparseMatrix<double> generateStiffnessMatrix();

	//Perform the force integration
	void performExplicitEulerIntegration();

	void clearLocalBuffers();
	void copyToLocalBuffers();
	void updateObject();


private:
	TwoDimMesh* object_;
	CollisionPlane* collObj_;

	std::vector<Eigen::Vector2d> localVelocityBuffer_;
	std::vector<Eigen::Vector2d> localPositionBuffer_;
	std::vector<Eigen::Vector2d> newPositionBuffer_;
	std::vector<Eigen::Vector2d> newVelocityBuffer_;
	std::vector<Eigen::Vector2d> forces_;

	std::vector<Eigen::Matrix2d> rotationMatrix_;
	std::vector<Eigen::Matrix2d> stiffnessMatrixData_;

	Eigen::SparseMatrix<double> M;
	Eigen::VectorXd x0;
};

