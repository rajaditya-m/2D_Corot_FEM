#pragma once

#include <QGLWidget>

#include "RenderObject.h"

class CollisionPlane :
	public RenderObject
{
public:
	CollisionPlane(double a, double b, double c);
	~CollisionPlane(void);

	double a()						{ return normalLineEqn_[0];			}
	double b()						{ return normalLineEqn_[1];			}
	double c()						{ return normalLineEqn_[2];			}

	void render(int frameIdx);

private:
	Eigen::Vector3d normalLineEqn_;
	Eigen::Vector2d renderEP1;
	Eigen::Vector2d renderEP2;
};

