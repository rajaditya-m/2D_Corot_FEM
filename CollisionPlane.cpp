#include "CollisionPlane.h"


CollisionPlane::CollisionPlane(double a, double b, double c)
{
	normalLineEqn_ = Eigen::Vector3d(a,b,c);
	double x = -8.0;
	double y = (-c-(a*x))/b;
	renderEP1 = Eigen::Vector2d(x,y);
	x = 8.0;
	y = (-c-(a*x))/b;
	renderEP2 = Eigen::Vector2d(x,y);
}


CollisionPlane::~CollisionPlane(void)
{
}

void CollisionPlane::render(int idx)
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glDisable(GL_LIGHTING);
	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
	glVertex3d(renderEP1.x(),renderEP1.y(),0.0);
	glVertex3d(renderEP2.x(),renderEP2.y(),0.0);
	glVertex3d(renderEP2.x(),renderEP2.y()+0.07,0.0);
	glVertex3d(renderEP1.x(),renderEP1.y()+0.07,0.0);
	glEnd();
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}
