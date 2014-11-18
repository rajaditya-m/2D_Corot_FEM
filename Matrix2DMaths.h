#pragma once

#include <Eigen/Dense>
#include <vector>

//Returns the matrix Q

inline Eigen::Matrix2d performQRDecomposition(Eigen::Matrix2d m)
{
	Eigen::Vector2d col0 = Eigen::Vector2d(m(0,0),m(1,0));
	col0.normalized();
	Eigen::Matrix2d Q;
	Q(0,0) = col0.x(); Q(0,1) = -col0.y();
	Q(0,1) = col0.y(); Q(1,1) = col0.x();
	return Q;
}

//Makes a flat columb vector from a vector of vectors 
inline Eigen::VectorXd flattenToColumnVector(std::vector<Eigen::Vector2d> vec)
{
	int numPoints = vec.size();
	Eigen::VectorXd retVec(numPoints*2);
	for(int i=0;i<numPoints;i++)
	{
		retVec(2*i)		=	vec[i].x();
		retVec(2*i+1)	=	vec[i].y();
	}
	return retVec;
}