#include "CentralConfig.h"

boost::property_tree::ptree
CentralConfig::ReadConfiguration(std::string filename)
{
	ptree tree;
	read_xml(filename, tree);

	return tree;
}