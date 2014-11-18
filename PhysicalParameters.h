#pragma once

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class PhysicalParameters
{
public:
	//The constructor to read the XML file and parse the result 
	PhysicalParameters(const char* property_xml);
	~PhysicalParameters(void);

	//Accessor methods 
	float get_youngs_modulus() const				{ return youngs_modulus;				}
	float get_poisson_ratio() const					{ return poisson_ratio;					}
	float get_timestep() const						{ return time_step;						}
	float get_thickness() const						{ return thickness;						}
	float get_max_solver_iteration() const			{ return max_solver_iterations;			}
	float getFriction() const						{ return frictionCoeff_;				}
	float get_damping_param() const					{ return damping_param;					}
	float getDensity() const						{ return density;						}
	double getRadialSpringStiffness() const			{ return radialSpringStiffness_;		}
	double getCrossSpringStiffness() const			{ return crossSpringStiffness_;			}
	double getCircumferalSpringStiffness() const	{ return circumferalSpringStiffness_;	}
	double getLameLambda() const					{ return lameLambda_;					}
	double getLameMu() const						{ return lameMu_;						}
	double getLameDampness() const					{ return lameDampness_;					}

private:
	float youngs_modulus;
	float poisson_ratio;
	float time_step;
	float thickness;
	float max_solver_iterations;
	float frictionCoeff_;
	float damping_param;
	float density;

	double radialSpringStiffness_;
	double crossSpringStiffness_;
	double circumferalSpringStiffness_;

	double lameLambda_;
	double lameMu_;
	double lameDampness_;


};

