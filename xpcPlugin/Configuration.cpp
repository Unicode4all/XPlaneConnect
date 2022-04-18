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

	void Configuration::setModified(bool m)
	{
		this->modified = m;
	}

	bool Configuration::getModified()
	{
		return this->modified;
	}

	bool Configuration::isAutosave() {
		return this->autosave;
	}

	void Configuration::setAutosave(bool a)
	{
		autosave = a;
	}



	void Configuration::setLogLevel(int level)
	{
	//tree->
	}

	bool Configuration::isInBlacklist(std::string icao)
	{
		auto c = tree.get_child_optional(fmt::format("blacklist.{}", icao));
		if (c.is_initialized())
			return true;
		else
			return false;
	}

	const std::string& Configuration::getCurrentLogFile()
	{
		return this->currentFile;
	}

	void Configuration::addToBlacklist(std::string icao, std::string desc, std::string author, bool fuel, bool payload)
	{
		auto tempTree = pt::ptree();
		auto& bl = tree.get_child("blacklist");	
		
		tempTree.put("desc", desc);
		tempTree.put("author", author);
		tempTree.put("fuel", fuel);
		tempTree.put("payload", payload);
		
		try {
			bl.add_child(icao, tempTree);
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to set configuration value: \"{}\"", e.what()));
			return;
		}

		if (autosave)
			save();
	}

	bool Configuration::removeFromBlacklist(std::string icao)
	{
		auto ret = this->tree.get_child_optional("blacklist");
		if (!ret.is_initialized()) {
			return false;
		}
		ret.get().erase(icao);
		if (autosave)
			save();
		return true;
	}

	BlacklistItem Configuration::getBlacklistItem(int id)
	{
		std::string icao;
		std::string desc;
		std::string author;
		
		try {
			
			icao = this->tree.get<std::string>(fmt::format("blacklist.{}.icao", id));
			id = this->tree.get<int>(fmt::format("blacklist.{}.id", id));
			desc = this->tree.get<std::string>(fmt::format("blacklist.{}.desc", id));
			author = this->tree.get<std::string>(fmt::format("blacklist.{}.author", id));
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to retrieve configuration value: \"{}\"", e.what()));
			return BlacklistItem {};
		}
	}

	std::vector<BlacklistItem> Configuration::getBlacklist()
	{
		std::vector<BlacklistItem> blacklist;
		try {
			for (auto& item : this->tree.get_child("blacklist")) {
				BlacklistItem i;
				i.desc = item.second.get<std::string>("desc");
				i.icao = item.first;
				i.author = item.second.get<std::string>("author");
				i.fuel = item.second.get<bool>("fuel");
				i.payload = item.second.get<bool>("payload");
				
				blacklist.push_back(i);
			}
		}
		catch (std::exception& e) {
			return std::vector<BlacklistItem>();
		}
		
		return blacklist;
	}

	void Configuration::setPort(uint16_t port)
	{
		if (port == 0)
			return;
		
		try {
			this->tree.put("server.port", port);
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to set configuration value: \"{}\"", e.what()));
		}
		if (autosave)
			save();
	}

	void Configuration::setOpsPerCycle(int opsPerCycle)
	{
		if (opsPerCycle == 0)
			return;
		
		try {
			this->tree.put("server.ops_per_cycle", opsPerCycle);
		}
		catch (pt::ptree_error& e) {
			XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Failed to set configuration value: \"{}\"", e.what()));
		}
		if (autosave)
			save();
	}

	void Configuration::attachWindow(std::shared_ptr<ConfigObserver> wnd)
	{
		configWindows.push_back(std::move(wnd));
	}

	pt::ptree& Configuration::getTree()
	{
		return tree;
	}

	void Configuration::notifyWindows()
	{
		if (!configWindows.empty()) {
			for (auto& wnd : configWindows)
			{
				wnd->UpdateConfig();
			}
		}
	}

	void Configuration::createDefaultConfig()
	{
		tree.put("server.port", defaultPort);
		tree.put("server.ops_per_cycle", defaultOpsPerCycle);
		tree.put("logger.level", LOG_ERROR);
		tree.add("blacklist", "");
		save();
	}

	void Configuration::load(const std::string filename)
	{
		XPC::Log::WriteLine(LOG_INFO, "CONF", fmt::format("Trying to load config \"{}\"", filename));
		std::fstream configFile(filename);

		// create default config if can't open existing (assuming file missing)
		if (!configFile.good())
			createDefaultConfig();

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
		notifyWindows();
	}

	void Configuration::save(const std::string filename)
	{
		std::ofstream configFile(filename, std::ios::out);
		XPC::Log::WriteLine(LOG_TRACE, "CONFIG", fmt::format("Trying to save config \"{}\"", filename));
		if (configFile.good()) {
			try {
				pt::write_info(configFile, this->tree);
			}
			catch (const pt::ptree_error& e) {
				XPC::Log::WriteLine(LOG_ERROR, "CONF", fmt::format("Failed to save config \"{}\": {}", filename, e.what()));
			}

			configFile.close();
		}
	}
}