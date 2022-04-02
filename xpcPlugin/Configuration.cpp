#include "Configuration.h"

namespace configuration {
	Configuration::Configuration()
	{
		this->tree = pt::ptree();
		// put default values onto tree so we could create a default config
		this->tree.put("server.port", defaultPort);
		this->tree.put("server.ops_per_cycle", defaultOpsPerCycle);
	}

	Configuration::~Configuration()
	{

	}

	uint16_t Configuration::getPort()
	{
		try {
			return this->tree.get<uint16_t>("server.port");
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to retrieve configuration value: \"{}\"", e.what()));
			return defaultPort;
		}
	}

	int Configuration::getOpsPerCycle()
	{
		try {
			return this->tree.get<uint16_t>("server.ops_per_cycle");
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to retrieve configuration value: \"{}\"", e.what()));
			return defaultPort;
		}
	}

	void Configuration::setPort(uint16_t port)
	{
		try {
			this->tree.put("server.port", port);
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to set configuration value: \"{}\"", e.what()));
		}
	}

	void Configuration::setOpsPerCycle(int opsPerCycle)
	{
		try {
			this->tree.put("server.ops_per_cycle", opsPerCycle);
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to set configuration value: \"{}\"", e.what()));
		}
	}

	void Configuration::load(const std::string filename)
	{
		XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Trying to load config \"{}\"", filename));
		std::fstream configFile(filename);

		// create default config if can't open existing (assuming file missing)
		if (!configFile.good()) {
			configFile.open(filename, std::ios::out);
			try {
				pt::write_info(configFile, this->tree);
			}
			catch (const pt::ptree_error& e) {
				XPC::Log::WriteLine(LOG_ERROR, "CONF", fmt::format("Fauled to create default config: \"{}\"", e.what()));
			}
			configFile.close();
		}

		try {
			pt::ptree temporaryTree;
			pt::read_info(filename, temporaryTree);
			// check
			auto port = temporaryTree.get_optional<uint16_t>("server.port");
			auto opsPerCycle = temporaryTree.get_optional<int>("server.ops_per_cycle");

			if (port && opsPerCycle) {
				this->tree = temporaryTree;
			}
			else {
				XPC::Log::WriteLine(LOG_ERROR, "CONF", fmt::format("The configuration file \"{}\" is malformed, using default values", filename));
			}
		}
		catch (const std::exception& e) {
			XPC::Log::WriteLine(LOG_ERROR, "CONF", fmt::format("Failed to load config \"{}\": {}", filename, e.what()));
		}
	}

	void Configuration::save(const std::string filename)
	{
		std::fstream configFile(filename);
		XPC::Log::WriteLine(LOG_INFO, "CONFIG", fmt::format("Trying to save config \"{}\"", filename));
		configFile.open(filename, std::ios::out);
		try {
			pt::write_info(configFile, this->tree);
		}
		catch (const pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_ERROR, "CONF", fmt::format("Failed to save config \"{}\": {}", filename, e.what()));
		}
		configFile.close();
	}
}