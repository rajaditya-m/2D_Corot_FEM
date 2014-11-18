#include "Scene.h"


Scene::Scene(bool twoDimScene)
{
	//Scene data
	xRot_ = yRot_ = zRot_ = 0;
	scaling_ = 3.0f;

	//Eye Data Default 
	if(scenetype_==THREE_DIM)
	{
		eyePos_ = Eigen::Vector3d(10.0f,5.0f,5.0f);
		centerPos_ = Eigen::Vector3d(0.0f,0.0f,0.0f);
		upDirection_ = Eigen::Vector3d(0.0f,1.0f,0.0f);
	}
	else
	{
		//eyePos_ = Eigen::Vector3d(0.0,0.0,17.0);
		eyePos_ = Eigen::Vector3d(0.0,-8.0,1.0);
		centerPos_ = Eigen::Vector3d(0.0,-8.0,0.0);
		upDirection_ = Eigen::Vector3d(0.0f,1.0f,0.0f);
	}

	//Lighting Data Default 
	lightPos1_ = Eigen::Vector3d(10.0f,25.0f,2.0f);

	//Current RenderFrame
	renderFrame_ = 0;

	//Default rendering data 
	renderGround_ = false;
	renderAxes_ = true;

	textureData_ = NULL;

	if(twoDimScene)
		scenetype_ = TWO_DIM;
	else
		scenetype_ = THREE_DIM;
}


Scene::~Scene(void)
{
}

void Scene::setAllRotations(int x,int y, int z)
{
	xRot_ = x;
	yRot_ = y;
	zRot_ = z;
}

void Scene::renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	glLoadIdentity();
	if(scenetype_==THREE_DIM)
	{
		glTranslatef(0.0, 0.0, -10.0);
		glRotatef(xRot_/16.0, 1.0, 0.0, 0.0);
		glRotatef(yRot_/16.0, 0.0, 1.0, 0.0);
		//gluLookAt(eyePos_.x(),eyePos_.y(),eyePos_.z(),centerPos_.x(),centerPos_.y(),centerPos_.z(),upDirection_.x(),upDirection_.y(),upDirection_.z());
	}
	gluLookAt(eyePos_.x(),eyePos_.y(),eyePos_.z(),centerPos_.x(),centerPos_.y(),centerPos_.z(),upDirection_.x(),upDirection_.y(),upDirection_.z());
	glScalef(scaling_,scaling_,scaling_);

	drawGradientBackGround();
	if(renderAxes_)
		drawAxes(16.0);
	if(renderGround_)
		drawGround();
	
	std::vector<RenderObject*>::iterator it;
	for(it = renderObj_.begin(); it!= renderObj_.end(); ++it)
	{
		(*it)->render(renderFrame_);
	}
	
}

void Scene::addRenderObject(RenderObject* obj)
{
	renderObj_.push_back(obj);
}

void Scene::setupScene()
{
	//Set up lighting and perspective data
	qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    static GLfloat lightPosition[4] = { lightPos1_.x(),lightPos1_.y(),lightPos1_.z(),1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	//Setup Ground Textures
	int imgHt,imgWd;
	textureData_ = readAsciiPPMImage("textures/woodfloortex.ppm",&imgHt,&imgWd);
	glGenTextures(1, &groundTex_);
	glBindTexture(GL_TEXTURE_2D, groundTex_);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,TEX_COLS,TEX_ROWS,0,GL_RGB,GL_UNSIGNED_BYTE,textureData_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Scene::resizeScene(int width, int height)
{
	width_ = width;
	height_ = height;
	glViewport(0,0,width,height);
	double aspectRatio = (double)width/(double)height;
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	if(scenetype_==TWO_DIM)
		glOrtho(-15.0 * aspectRatio, 15.0 * aspectRatio, -15.0, 15.0, 10.0, -10.0);
	else
		gluPerspective(60, aspectRatio, 1,1000);
	glMatrixMode(GL_MODELVIEW);
}

//@TODO: Custom Axis Labellign Colors
void Scene::drawAxes(float length)
{
    glPushMatrix();
	glColor3f(1.0,0.0,0.0);
    Arrow(0,0,0, length,0,0, 0.1);
    glPopMatrix();

    glPushMatrix();
	glColor3f(0.0,1.0,0.0);
    Arrow(0,0,0, 0,length,0, 0.1);
    glPopMatrix();

	if(scenetype_==THREE_DIM)
	{
		glPushMatrix();
		glColor3f(0.0,0.0,1.0);
		Arrow(0,0,0, 0,0,length, 0.1);
		glPopMatrix();
	}

	if(scenetype_==TWO_DIM)
	{
		glPushMatrix();
		glColor3f(1.0,0.0,0.0);
		Arrow(0,0,0, -length,0,0, 0.1);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0,1.0,0.0);
		Arrow(0,0,0, 0,-length,0, 0.1);
		glPopMatrix();
	}
	
}

//@TODO: Custom Gradient Background
void Scene::drawGradientBackGround() 
{
	// Meshlab background colors (given by Xiaofeng )
	float lower_color[3] = {115 / 255.0f, 115 / 255.0f, 230 / 255.0f};
	float upper_color[3] = {0 / 255.0f, 0 / 255.0f, 0 / 255.0f};

	GLboolean lighting_enabled, depth_test_enabled;
	glGetBooleanv(GL_DEPTH_TEST, &depth_test_enabled);
	glGetBooleanv(GL_LIGHTING, &lighting_enabled);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);
	glColor3fv(lower_color);
	glVertex3f(-1.0,-1.0, 1);
	glVertex3f(1.0,-1.0, 1);
	glColor3fv(upper_color);
	glVertex3f(1.0, 1.0, 1);
	glVertex3f(-1.0, 1.0, 1);
	glEnd();

	if (lighting_enabled)
	{ 
		glEnable(GL_LIGHTING);
	}
	if (depth_test_enabled)
	{ 
		glEnable(GL_DEPTH_TEST);
	}

	glPopMatrix(); // Pop model view matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//@TODO : Custom Texture Function Selection
void Scene::drawGround()
{
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D,groundTex_);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glBegin(GL_QUADS);

	glColor3f(0.5,0.5,0.5);
	glTexCoord2f(0, 0);
	glVertex3f(50.0,0.0,-50.0);
 	glTexCoord2f(1, 0);
	glVertex3f(-50.0,0.0,-50.0);
	glTexCoord2f(1, 1);
	glVertex3f(-50.0,0.0,50.0);
	glTexCoord2f(0,1);
	glVertex3f(50.0,0.0,50.0);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

//Private Functions for internal usage only

void Scene::Arrow(float x1,float y1,float z1,float x2,float y2,float z2,float D)
{
	float x=x2-x1;
	float y=y2-y1;
	float z=z2-z1;
	float L=sqrt(x*x+y*y+z*z);

	GLUquadricObj *quadObj;

	glPushMatrix ();
	glDisable(GL_LIGHTING);
	glTranslated(x1,y1,z1);

	if((x!=0.)||(y!=0.))
	{
		glRotated(atan2(y,x)/RADPERDEG,0.,0.,1.);
		glRotated(atan2(sqrt(x*x+y*y),z)/RADPERDEG,0.,1.,0.);
	} 
	else if (z<0)
	{
		glRotated(180,1.,0.,0.);
	}
  
	glTranslatef(0,0,L-4*D);

	//Sketches the end cone
	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, 0.65*D, 0.0, 4*D, 32, 1);
	gluDeleteQuadric(quadObj);
  
	glTranslatef(0,0,-L+4*D);

	//Sketches the rod
	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, 0.25*D, 0.25*D, L-4*D, 32, 1);
	gluDeleteQuadric(quadObj);
		
	//Sketches the end sealing disc
	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH);
	gluDisk(quadObj, 0.0, 0.25*D, 32, 1);
	gluDeleteQuadric(quadObj);
	glEnable(GL_LIGHTING);
    glPopMatrix ();
}

GLubyte* Scene::readAsciiPPMImage(char* fName,int* height,int* width) 
{
		
	FILE* in = fopen(fName, "r"); 

	int tht,twt;

	  	int  ccv; 
	  	char header[100]; 
	  	fscanf(in, "%s %d %d %d", header, &twt, &tht, &ccv); 
	  	int r, g, b; 

	  	*height = tht;
	  	*width = twt;

	  	GLubyte* texImg = new GLubyte[tht*twt*3];
		GLubyte readImage[500][500][3]; 

	  	for (int i=tht-1; i>=0; i--)
	  	{
	  		for (int j=0; j<twt; j++)
			{
	      		fscanf(in, "%d %d %d", &r, &g, &b); 
	      		readImage[i][j][0] = (GLubyte)r; 
	      		readImage[i][j][1] = (GLubyte)g; 
	      		readImage[i][j][2] = (GLubyte)b; 
	    	}
	  	}
	     	
	  	for (int i=0; i<tht; i++)
	  	{
	    	for ( int j=0; j<twt; j++)
	    	{
	    		texImg[i*twt*3+j*3+0] = readImage[i][j][0];
				texImg[i*twt*3+j*3+1] = readImage[i][j][1];
				texImg[i*twt*3+j*3+2] = readImage[i][j][2];
	    	}
	  	}	  
	  	fclose(in);
	  	return texImg; 
}
