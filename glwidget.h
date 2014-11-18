#pragma once

#define EIGEN_USE_MKL_ALL

#include <QGLWidget>
#include <QtOpenGL>
#include <qstring.h>

#include <gl/glu.h>
#include <math.h>

#include "global_typedefs.h"
#include "Scene.h"
#include "TwoDimMesh.h"
#include "TwoDimFEMEngine.h"
#include "CollisionPlane.h"

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent=0);
	~GLWidget();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width,int height);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

public slots:
	void setRenderingFrameNumber(double frame_number);
	void setRenderingFrameNumber(int frame_number);
	void startAnimation();
	void pauseAnimation();
	void resetAnimation();
	void nextFrame();

	//void generateData();

	//void setPerturbValue(int val);

	void renderAxesToggled(bool toggle);
	//void renderSpringsToggled(bool toggle);
//	void perturbSingle();
	//void perturbWeighted();

signals:
	void frameNumberUpdated(double frame_nos);
	void frameNumberUpdated(int frame_nos);
	void animFrameNumberUpdated(int frame_nos);


private:
	PhysicalParameters* param; // @TODO : Will be shifted later on to the tube class for more consistent representation 

	TwoDimMesh* object_;
	CollisionPlane* plane_;

	//Simulation_Engine* simEngine_;
	TwoDimFEMEngine* simEngine_;

	//Animation timer
	QTimer* animation_timer;


	//Scence Data 
	Scene* scene_;

};
