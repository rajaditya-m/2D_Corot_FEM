#include "RenderObject.h"

RenderObject::RenderObject()
{
	rotationInvariant_ = false;
	render_ = true;
	renderMode_ = SHADING;
}

RenderObject::RenderObject(bool rotInv)
{
	rotationInvariant_ = rotInv;
	render_ = true;
	renderMode_ = SHADING;
}

RenderObject::~RenderObject()
{
}

void RenderObject::render(int frameId)
{
	//Nothing to do here 
	// Will be populated by the abstyract class
}