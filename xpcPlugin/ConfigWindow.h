#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"
#include "Configuration.h"
#include "ImgWindow.h"
#include "PluginDefs.h"
#include "ConfigObserver.h"
#include "DataManager.h"
#include "Log.h"

#include <fmt/format.h>

#include <charconv>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

using std::ios;
using std::vector;
using std::string;

#define COLOR_ERROR ImColor(200, 100, 100)
#define COLOR_INFO	ImColor(100, 200, 100)
#define COLOR_TRACE	ImColor(100, 100, 200)

class ConfigWindow : public ImgWindow, public ConfigObserver
{
public:
	ConfigWindow(std::shared_ptr<configuration::Configuration> const& config,
		int left,
		int top,
		int right,
		int bottom);

	void setAutoScroll(bool s);
	bool isAutoScroll();
	virtual bool onShow() override;	
	virtual void UpdateConfig() override;
protected:
	vector<int>& GetLogLineOffsets();
	GLuint xpcLogo = 0;
	GLuint nasaLogo = 0;
	ImVec2 xpclSize;
	ImVec2 nasalSize;

	ImGuiTextBuffer LogBuf;
	ImGuiTextFilter LogFilter;
	

	bool AutoScroll = true;
	bool autosave = true;

	int cycles = 2000;
	int logSeek = 0;
private:
	vector<int> LogLineOffsets;
	std::shared_ptr<configuration::Configuration> config;
	std::unique_ptr<vector<string>> logBuf;
	std::array<char, 6> portBuf = {0};
	std::array<char, 3> opsBuf = {0};
	std::string currentConfigFile = "";
	std::map<XPC::DREF, XPLMDataRef> datarefs;
	// Inherited via ImgWindow
	virtual void buildInterface() override;
	static int portInputCB();
	
	bool autoScroll = true;
	static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
};

#endif