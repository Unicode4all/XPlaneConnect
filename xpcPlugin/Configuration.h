#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cstdint>
#include <format>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "Log.h"

namespace pt = boost::property_tree;

namespace configuration {
	
constexpr uint16_t defaultPort = 49009;
constexpr int defaultOpsPerCycle = 40;
constexpr auto defaultConfigName = "XPConnectConfig.cfg";
/// <summary>
/// Configuration stores plugin configuration parameters such as socket port
/// </summary>
class Configuration
{
public:
	Configuration();
	~Configuration();
	
	uint16_t getPort();
	int getOpsPerCycle();

	void setPort(uint16_t port);
	void setOpsPerCycle(int opsPerCycle);
	
	void load(const std::string filename = defaultConfigName);
	void save(const std::string filename = defaultConfigName);
private:
	pt::ptree tree;

	uint16_t port = 49009;
	int opsPerCycle = 40;
};
}

#endif