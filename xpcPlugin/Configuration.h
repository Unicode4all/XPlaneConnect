#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Log.h"
#include "ConfigObserver.h"

#include <cstdint>
#include <fmt/core.h>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <vector>

#define ADD_TO_TREE(type, path, value) 
namespace pt = boost::property_tree;

namespace configuration {
	
constexpr uint16_t defaultPort = 49009;
constexpr int defaultOpsPerCycle = 40;
constexpr auto defaultConfigName = "XPConnectConfig.cfg";
/// <summary>
/// Configuration stores plugin configuration parameters such as socket port
/// </summary>



enum BlacklistFlags {
	Fuel = 1,
	Payload
};

typedef struct {
	std::string icao;
	std::string desc;
	std::string author;

	bool	fuel;
	bool	payload;
} BlacklistItem;

class Configuration
{
public:
	Configuration();
	~Configuration();
	
	uint16_t getPort();
	int getOpsPerCycle();

	void setModified(bool m);
	bool getModified();
	bool isAutosave();
	void setAutosave(bool a);

	void setLogLevel(int level);

	bool isInBlacklist(std::string icao);

	template<class T>
	void setOption(std::string path, T option) {
		try {
			this->tree.put(path, option);
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to set configuration value: \"{}\"", e.what()));
		}
		if (autosave)
			save();
	}

	template<class T>
	T getOption(std::string path) {
		try {
			return this->tree.get<T>(path);
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to retrieve configuration value: \"{}\"", e.what()));
			return T();
		}
	}
	
	int getLogLevel();

	const std::string& getCurrentLogFile();

	void addToBlacklist(std::string icao, std::string desc, std::string author, bool fuel = true, bool payload = false);
	bool removeFromBlacklist(std::string icao);
	BlacklistItem getBlacklistItem(int id);	
	std::vector<BlacklistItem> getBlacklist();
	void setPort(uint16_t port);
	void setOpsPerCycle(int opsPerCycle);

	void attachWindow(std::shared_ptr<ConfigObserver> wnd);

	pt::ptree& getTree();
	
	void load(const std::string filename = defaultConfigName);
	void save(const std::string filename = defaultConfigName);
private:
	void notifyWindows();
	void createDefaultConfig();
	bool modified = false;
	bool autosave = true;
	pt::ptree tree;
	std::string currentFile;
	std::vector<std::shared_ptr<ConfigObserver>> configWindows;
};
}

#endif