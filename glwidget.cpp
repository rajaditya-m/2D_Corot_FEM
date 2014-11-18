#include "glwidget.h"

//Using stuff in Clothing_3 before

GLWidget::GLWidget(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	//Initialize scene
	scene_ = new Scene(true);

	//Initialize the physical properties 
	PhysicalParameters* param = new PhysicalParameters("physical_param.xml");

	//Initialize the circle
	object_ = new TwoDimMesh("q1.obj","physical_param.xml");
	scene_->addRenderObject(object_);

	//Inittialze the collision plane
	plane_ = new CollisionPlane(0.0,1.0,7.0);
	scene_->addRenderObject(plane_);

	//Initialize the simulation engine
	//simEngine_ = new Simulation_Engine(circleTest_);
	simEngine_ = new TwoDimFEMEngine(object_,plane_);

	//Animaton timer
	animation_timer = new QTimer(this);
	connect(animation_timer, SIGNAL(timeout()), this, SLOT(nextFrame()));

	//If data generation needed
	//generateData();

	//Multimaterial tube test
	//mmtube_ = new MultiMaterialTube(1.0,2.0,30);
	//mmtube_->generateCompletePOLY("MultiMaterialThin.poly");
	//mmtube_->generateTetsProcedurally("procgentube");
}

/*
void GLWidget::generateData()
{
	int numFrames = 80;
	double scalingSingleMin = 0.05;
	double scalingSingleMax = 0.98;
	double scalingSingleOffset = 0.01;
	double scalingWeightedMin = 0.20;
	double scalingWeightedMax = 0.98;
	double scalingWeightedOffset = 0.05;
	std::ofstream file_output("C:/Users/mukherjr/Copy/data_10_16_2014.txt");
	//Do this for each sample 
	for(int i=0;i<circleTest_->getNumberSamples();i++)
	{
		std::cout << "Processing Sample number#" << i << "\n";
		//For each point we have to do first a single point weighting 
		double scalingSingle = scalingSingleMin;
		while(scalingSingle<=scalingSingleMax)
		{
			//Perturb the points
			circleTest_->perturbOuterCircle(i,scalingSingle);
			//Generate 80 frames
			for(int j=0;j<numFrames;j++)
			{
				simEngine_->generateNextFrame();
			}
			//Dump all the data to the main file
			for(int f=0;f<numFrames;f++)
			{
				for(int j=0;j<circleTest_->getNumberSamples();j++)
				{
					Eigen::Vector2d pnt = circleTest_->getCircle(f,0)->getSample(j);
					file_output << pnt.x() << " " << pnt.y() << " "; 
				}
				file_output << "\n";
			}
			
			//Reset the thing for the next one 
			circleTest_->resetValues();

			//Increment scaling parameter
			scalingSingle +=scalingSingleOffset;
		}
		//For each point, we have to then do a weighted scaling 
		double scalingWeighted = scalingWeightedMin;
		while(scalingWeighted<=scalingWeightedMax)
		{
			for(int sc = 4 ; sc<=20 ;sc+=2)
			{
				//Perturb the points
				circleTest_->weightedPerturbOuterCircle(i,scalingWeighted,sc);
				//Generate 80 frames
				for(int j=0;j<numFrames;j++)
				{
					simEngine_->generateNextFrame();
				}
				//Dump all the data to the main file
				for(int f=0;f<numFrames;f++)
				{
					for(int j=0;j<circleTest_->getNumberSamples();j++)
					{
						Eigen::Vector2d pnt = circleTest_->getCircle(f,0)->getSample(j);
						file_output << pnt.x() << " " << pnt.y() << " "; 
					}
					file_output << "\n";
				}
			
				//Reset the thing for the next one 
				circleTest_->resetValues();
			}
			scalingWeighted += scalingWeightedOffset;
		}
		
	}
	file_output.close();
}
*/

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::initializeGL()
{
	scene_->setupScene();
}

void GLWidget::paintGL()
{
	scene_->renderScene();
}

void GLWidget::resizeGL(int width,int height)
{
	scene_->resizeScene(width,height);
}

static void qNormalizeAngle(int &angle)
{
	while(angle < 0)
	{
		angle += 360*16;
	}
	while(angle > 360*16)
	{
		angle -= 360*16;
	}
}

//Rendering options 
void GLWidget::renderAxesToggled(bool val)
{
	scene_->setRenderAxes(val);
	updateGL();
}

/*
void GLWidget::renderSpringsToggled(bool val)
{
	circleTest_->setRenderSprings(val);
	updateGL();
}

void GLWidget::perturbSingle()
{
	circleTest_->perturbOuterCircle(perturbIndex_);
	updateGL();
}

void GLWidget::perturbWeighted()
{
	circleTest_->weightedPerturbOuterCircle(perturbIndex_);
	updateGL();
}
void GLWidget::setPerturbValue(int val)
{
	perturbIndex_ = val;
	std::cout << "Perturb Value changed to " << perturbIndex_ << "\n";
}
*/

void GLWidget::setRenderingFrameNumber(double frame_number)
{
	int frameNos = static_cast<int>(frame_number);
	scene_->setCurrentRenderFrame(frameNos);
	int crossSectionFrameAvailable = object_->numFrames()-1;
	if(frameNos>crossSectionFrameAvailable)
	{
		int diff = frameNos-crossSectionFrameAvailable;
		for(int num = 0; num < diff ; num++)
			simEngine_->generateNextFrame();
	}
	emit animFrameNumberUpdated(frame_number);
	updateGL();
}

void GLWidget::setRenderingFrameNumber(int frame_number)
{
	int frameNos = static_cast<int>(frame_number);
	scene_->setCurrentRenderFrame(frameNos);
	int crossSectionFrameAvailable = object_->numFrames()-1;
	if(frameNos>crossSectionFrameAvailable)
	{
		int diff = frameNos-crossSectionFrameAvailable;
		for(int num = 0; num < diff ; num++)
			simEngine_->generateNextFrame();
	}
	emit frameNumberUpdated((double)frameNos);
	updateGL();
}

void GLWidget::startAnimation()
{
	animation_timer->start(30);
}

void GLWidget::pauseAnimation()
{
	animation_timer->stop();
}

void GLWidget::resetAnimation()
{
	animation_timer->stop();
	scene_->setCurrentRenderFrame(0);
	emit animFrameNumberUpdated(0);
	emit frameNumberUpdated(0.0); 
	//circleTest_->resetValues();
	updateGL();
}

void GLWidget::nextFrame()
{
	int renderFrame = scene_->getCurrentRenderFrame();
	renderFrame++;
	//if(renderFrame==301)
	//	renderFrame = 0;
	scene_->setCurrentRenderFrame(renderFrame);
	int crossSectionFrameAvailable = object_->numFrames()-1;
	if(renderFrame>crossSectionFrameAvailable)
	{
		int diff = renderFrame-crossSectionFrameAvailable;
		for(int num = 0; num < diff ; num++)
			simEngine_->generateNextFrame();
	}
	emit animFrameNumberUpdated(renderFrame);
	emit frameNumberUpdated((double)renderFrame);
	updateGL();
	QString format = "png";
	QString filename = QString("ScreenCast_%1.png").arg(renderFrame);
    QString initialPath = QDir::currentPath() + tr("/Screenshots/")  + filename;
     if (!initialPath.isEmpty())
		 this->grabFrameBuffer().save(initialPath);
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	scene_->setLastPosition(event->pos());
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	QPoint cur_pos = event->pos();
	int xRot = scene_->getXRot();
	int yRot = scene_->getYRot();
	float scaling = scene_->getScaling();

	QPoint last_pos = scene_->getLastPosition();
	QPoint diff = cur_pos - last_pos;
	
	if(event->buttons() & Qt::LeftButton)
	{
		int x_angle = xRot + 8*diff.y();
		qNormalizeAngle(x_angle);
		if(x_angle != xRot)
		{
			scene_->setXRot(x_angle);
		}
		int y_angle = yRot + 8*diff.x();
		qNormalizeAngle(y_angle);
		if(y_angle != yRot)
		{
			scene_->setYRot(y_angle);
		}
	}
	else if(event->buttons() & Qt::RightButton)
	{
		float old_size = scaling;
		scaling *= (1 + (diff.y())/60.0);
		scene_->setScaling(scaling);
		if (scaling <0)
		{
			scene_->setScaling(old_size);
		}
	}
	updateGL();
	scene_->setLastPosition(cur_pos);
}


