#ifndef XMLREADERS_H_
#define XMLREADERS_H_

#include <vector>

#include <Eigen/Dense>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

std::vector<Eigen::Vector3d> populatePointsFromXML(const char* file);

std::vector<float> populateScalarsFromXML(const char* file);



#endif