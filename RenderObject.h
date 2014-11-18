#pragma once

#include "mesh_material.h"
#include "global_typedefs.h"

//A renderable object has the following things in it :
// A mesh and a material xml file 
// It must have a render function which changes from frame to frame 


class RenderObject 
{
public:
	RenderObject();
	RenderObject(bool rotInv);
	~RenderObject();

	virtual void render(int frameId);

	void setRenderable(bool render)				{ render_ = render;				}
	bool isRenderable() const					{ return render_;				}

	void setRenderMode(RenderMode rendermode)	{ renderMode_ = rendermode;		}
	RenderMode getRenderMode() const			{ return renderMode_;			}

	bool isRotationInvariant() const			{ return rotationInvariant_;	}

protected:
	bool render_;
	RenderMode renderMode_;
	bool rotationInvariant_;
};