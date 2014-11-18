#pragma once

#include <sstream>

#include <QGLWidget>

#include "RenderObject.h"
#include "PhysicalParameters.h"
#include "triangles.h"

class TwoDimMesh : public RenderObject
{
public:
	TwoDimMesh(const char* initOBJ,const char* paramFile);
	~TwoDimMesh(void);
	void render(int frameId);

	int numPoints()														{ return position_[0].size();		}
	int numTriangles()													{ return triangles_.size();			}
	int numFrames()														{ return position_.size();			}
	PhysicalParameters* getParams()										{ return params_;					}
	double getMass(int idx)												{ return mass_[idx];				}
	std::vector<Eigen::Vector2d> getVelocityVector(int fidx)			{ return velocity_[fidx];			}
	std::vector<Eigen::Vector2d> getPositionVector(int fidx)			{ return position_[fidx];			}
	Eigen::Vector2d getVelocity(int frameIdx, int pIdx)					{ return velocity_[frameIdx][pIdx];	}
	Eigen::Vector2d getPosition(int frameIdx, int pIdx)					{ return position_[frameIdx][pIdx];	}
	Triangles getTriangle(int triIdx)									{ return triangles_[triIdx];		}
	Eigen::Matrix2d getPreComputedDm(int tIdx)							{ return Dm[tIdx];					}
	Eigen::Vector2d getAreaNormals(int id)								{ return areaWeighedNormals_[id];	}
	Eigen::Matrix2d getRestStiffnessMatrix(int idx)						{ return restStiffnessMatrix_[idx];	}
	Eigen::Vector2d getForceOffsets(int idx)							{ return forceOffsets_[idx];		}

	std::vector<int> getOffsetVector()									{ return offsets_;					}
	std::vector<int> getColumnVector()									{ return columns_;					}
	int getSizeOfColumnVector()											{ return columns_.size();			}

	int getLocationFromFront(int row, int col);


	void appendNewFramePosition(std::vector<Eigen::Vector2d> p)			{ position_.push_back(p);			}
	void appendNewFrameVelocity(std::vector<Eigen::Vector2d> v)			{ velocity_.push_back(v);			}

private:
	std::vector<std::vector<Eigen::Vector2d> >  position_;
	std::vector<std::vector<Eigen::Vector2d> > velocity_;
	std::vector<Triangles> triangles_;

	//Stuff we need precomputed for the distributions of FEM Solver 
	std::vector<Eigen::Matrix2d> Dm;
	std::vector<Eigen::Vector2d> areaWeighedNormals_;
	std::vector<Eigen::Matrix2d> restStiffnessMatrix_;
	std::vector<Eigen::Vector2d> forceOffsets_;

	//Some more stuff we need to assemble the stiffness matrix 
	std::vector<int> offsets_;
	std::vector<int> columns_;
	
	//Mass per particle 
	std::vector<double> mass_;

	//Area per triaangle 
	std::vector<double> area_;

	//Physical Parameters 
	PhysicalParameters* params_;
};

