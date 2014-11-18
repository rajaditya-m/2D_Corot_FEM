#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <QGLWidget>

class Mesh_Material
{
public:
	Mesh_Material(const char* material_prop_xml);
	~Mesh_Material(void);

	void set_material();	

	float get_shininess() const		{ return shininess;			}
	float* get_k_ambient()			{ return k_ambient;			}
	float* get_k_diffuse()			{ return k_diffuse;			}
	float* get_k_specular()			{ return k_specular;		}
	float* get_k_emmissive()		{ return k_emissive;		}

private:
	float shininess;
	float k_ambient[4];
	float k_diffuse[4];
	float k_specular[4];
	float k_emissive[4];
};

