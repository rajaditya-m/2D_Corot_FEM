#include "XMLReaders.h"

// Assume that the XML has a format as follows 
// <Document>
//	<Point> 
//		<x>12.0</x> 
//		<y>13.0</y>
//		<z>0.0</y>
//	</Point>
// <Document>

std::vector<Eigen::Vector3d> populatePointsFromXML(const char* file)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml(file,pt);

	std::vector<Eigen::Vector3d> pointVec;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("Document"))
	{
		Eigen::Vector3d point;
		//boost::property_tree::ptree childPtr = v.second;
		point(0) = v.second.get<float>("x");
		point(1) = v.second.get<float>("y");
		point(2) = v.second.get<float>("z");
		pointVec.push_back(point);
	}

	return pointVec;
}

//Assume that the XML has a format as folllows
// <Document>
// <Scalar> 12.0 </Scalar>
// <Scalar> 13.0 </Scalar>
// </Document>

std::vector<float> populateScalarsFromXML(const char* file)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml(file,pt);

	std::vector<float> scalarVec;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("Document"))
	{
		float tempVal = v.second.get<int>("Scalar");
		scalarVec.push_back(tempVal);
	}

	return scalarVec;
}