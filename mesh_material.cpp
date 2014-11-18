#include "mesh_material.h"


Mesh_Material::Mesh_Material(const char* material_prop_xml)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml(material_prop_xml,pt);

	shininess = pt.get<float>("material.shine");

	k_ambient[0] = pt.get<float>("material.ka.r");
	k_ambient[1] = pt.get<float>("material.ka.b");
	k_ambient[2] = pt.get<float>("material.ka.b");
	k_ambient[3] = pt.get<float>("material.ka.a");

	k_diffuse[0] = pt.get<float>("material.kd.r");
	k_diffuse[1] = pt.get<float>("material.kd.b");
	k_diffuse[2] = pt.get<float>("material.kd.b");
	k_diffuse[3] = pt.get<float>("material.kd.a");

	k_specular[0] = pt.get<float>("material.ks.r");
	k_specular[1] = pt.get<float>("material.ks.b");
	k_specular[2] = pt.get<float>("material.ks.b");
	k_specular[3] = pt.get<float>("material.ks.a");

	k_emissive[0] = pt.get<float>("material.ke.r");
	k_emissive[1] = pt.get<float>("material.ke.b");
	k_emissive[2] = pt.get<float>("material.ke.b");
	k_emissive[3] = pt.get<float>("material.ke.a");
}


Mesh_Material::~Mesh_Material(void)
{
}

void Mesh_Material::set_material()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  k_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  k_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  k_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  k_emissive);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);
}
