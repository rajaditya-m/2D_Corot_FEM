#include "PhysicalParameters.h"


PhysicalParameters::PhysicalParameters(const char* property_xml)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml(property_xml,pt);

	//Now read everything one by one 
	youngs_modulus = pt.get<float>("property.youngs_modulus");
	poisson_ratio = pt.get<float>("property.poisson_ratio");
	time_step = pt.get<float>("property.time_step");
	thickness = pt.get<float>("property.thickness");
	max_solver_iterations = pt.get<float>("property.max_iter");
	frictionCoeff_ = pt.get<float>("property.friction_coeff");
	damping_param = pt.get<float>("property.damping_param");
	density = pt.get<float>("property.density");
	radialSpringStiffness_ = pt.get<double>("property.radial_spring_stiffness");
	circumferalSpringStiffness_ = pt.get<double>("property.circumeferal_spring_stiffness");
	crossSpringStiffness_ = pt.get<double>("property.cross_spring_stiffness");
	lameLambda_ = pt.get<double>("property.lameLambda");
	lameMu_ = pt.get<double>("property.lameMu");
	lameDampness_ = pt.get<double>("property.lameDampness");

}


PhysicalParameters::~PhysicalParameters(void)
{
}
