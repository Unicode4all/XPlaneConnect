#include "Configuration.h"

namespace configuration {
	Configuration::Configuration()
	{
		this->tree = pt::ptree();
		// put default values onto tree so we could create a default config
		this->tree.put("server.port", defaultPort);
		this->tree.put("server.ops_per_cycle", this->opsPerCycle);
	}

	Configuration::~Configuration()
	{

	}

	uint16_t Configuration::getPort()
	{
		return this->tree.get<uint16_t>("server.port");
	}

	int Configuration::getOpsPerCycle()
	{
		return this->tree.get<uint16_t>("server.ops_per_cycle");
	}

	void Configuration::setPort(uint16_t port)
	{
		this->tree.put("server.port", port);
	}

	void Configuration::setOpsPerCycle(int opsPerCycle)
	{
		this->tree.put("server.ops_per_cycle", opsPerCycle);
	}

	void Configuration::load(const std::string filename)
	{
		XPC::Log::WriteLine(LOG_INFO, "CONFIG", std::format("Trying to load config \"{}\"", filename));
		std::fstream configFile(filename);

		// create default config if can't open existing (assuming file missing)
		if (!configFile.good()) {
			configFile.open(filename, std::ios::out);
			try {
				pt::write_info(configFile, this->tree);
			}
			catch (const pt::ptree_error& e) {
				XPC::Log::WriteLine(LOG_ERROR, "CONFIG", std::format("Fauled to create default config: \"{}\"", e.what()));
			}
			configFile.close();
		}

		try {
			pt::read_info(filename, this->tree);

			this->port = this->tree.get<uint16_t>("server.port");
			this->opsPerCycle = this->tree.get<int>("server.ops_per_cycle");
		}
		catch (const std::exception& e) {
			XPC::Log::WriteLine(LOG_ERROR, "CONFIG", std::format("Failed to load config \"{}\": {}", filename, e.what()));
		}
	}

	void Configuration::save(const std::string filename)
	{
		std::fstream configFile(filename);
		XPC::Log::WriteLine(LOG_INFO, "CONFIG", std::format("Trying to save config \"{}\"", filename));
		configFile.open(filename, std::ios::out);
		try {
			pt::write_info(configFile, this->tree);
		}
		catch (const pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_ERROR, "CONFIG", std::format("Failed to save config \"{}\": {}", filename, e.what()));
		}
		configFile.close();
	}
}