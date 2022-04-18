#include "ConfigWindow.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define IMLOG(lineStart, lineEnd) auto s = std::string(lineStart, lineEnd);\
	if (s.find("INFO") != std::string::npos)\
		ImGui::TextColored(COLOR_INFO, s.c_str());\
	else if (s.find("ERROR") != std::string::npos)\
		ImGui::TextColored(COLOR_ERROR, s.c_str());\
	else if (s.find("TRACE") != std::string::npos)\
		ImGui::TextColored(COLOR_TRACE, s.c_str());\
	else\
		ImGui::TextUnformatted(s.c_str());\

#define LOG_LEVEL_FROM_STRING(level, current_item) if (!std::strcmp(current_item, loggingLevels[0])) level = LOG_OFF;\
				else if (!std::strcmp(current_item, loggingLevels[1])) level = LOG_ERROR;\
				else if (!std::strcmp(current_item, loggingLevels[2])) level = LOG_INFO;\
				else if (!std::strcmp(current_item, loggingLevels[3])) level = LOG_DEBUG;\
				else if (!std::strcmp(current_item, loggingLevels[4])) level = LOG_TRACE;\

#define LOG_LEVEL_TO_STRING(level, current_item) if (XPC::Log::GetLogLevel() == LOG_OFF) current_item = "None";\
				else if (XPC::Log::GetLogLevel() == LOG_ERROR) current_item = "Error";\
				else if (XPC::Log::GetLogLevel() == LOG_INFO) current_item = "Info";\
				else if (XPC::Log::GetLogLevel() == LOG_DEBUG) current_item = "Debug";\
				else if (XPC::Log::GetLogLevel() == LOG_TRACE) current_item = "Trace";\


ConfigWindow::ConfigWindow(std::shared_ptr<configuration::Configuration> const& config, int left, int top, int right, int bottom) : ImgWindow(left, top, right, bottom)
{
	datarefs.insert(std::pair(XPC::DREF_AcfAuthor, XPLMFindDataRef("sim/aircraft/view/acf_author")));
	datarefs.insert(std::pair(XPC::DREF_AcfICAO, XPLMFindDataRef("sim/aircraft/view/acf_ICAO")));
	datarefs.insert(std::pair(XPC::DREF_AcfDescription, XPLMFindDataRef("sim/aircraft/view/acf_descrip")));
	datarefs.insert(std::pair(XPC::DREF_AcfModeS, XPLMFindDataRef("sim/aircraft/view/acf_modeS_id")));

	this->config = config;
	SetWindowTitle("X-Plane Connect");
	SetWindowResizingLimits(600, 250, 800, 900);
	std::to_chars(portBuf.data(), portBuf.data() + portBuf.size(), this->config->getPort());
	std::to_chars(opsBuf.data(), opsBuf.data() + opsBuf.size(), this->config->getOpsPerCycle());
	LogBuf = ImGuiTextBuffer();
}

void ConfigWindow::setAutoScroll(bool s)
{
	autoScroll = s;
}

bool ConfigWindow::isAutoScroll()
{
	return autoScroll;
}

bool ConfigWindow::onShow()
{
	int x, y;
	LoadTextureFromFile("Resources/Plugins/XPlaneConnect/res/XPC.png", &xpcLogo, &x, &y);
	xpclSize = ImVec2(x, y);
	LoadTextureFromFile("Resources/Plugins/XPlaneConnect/res/nasa.png", &nasaLogo, &x, &y);
	nasalSize = ImVec2(x, y);
	return true;
}

void ConfigWindow::UpdateConfig()
{
	std::to_chars(portBuf.data(), portBuf.data() + portBuf.size(), this->config->getPort());
	std::to_chars(opsBuf.data(), opsBuf.data() + opsBuf.size(), this->config->getOpsPerCycle());
}

void ConfigWindow::buildInterface()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 5.0f;
	style.FramePadding = ImVec2(7, 5);
	style.FrameRounding = 5.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 3.0f;

	style.FrameBorderSize = 0;
	style.ChildBorderSize = 0;

	style.Alpha = 1.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImColor(0, 0, 0, 69);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.74f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.0f);

	if (ImGui::BeginTabBar("#confTabs", ImGuiTabBarFlags_None)) {
		std::vector<int>& logOffsets = GetLogLineOffsets();
		if (ImGui::BeginTabItem("Settings")) {
			ImGui::BeginChild("configuration_tab", ImVec2(0, -60), false, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Configuration");
			if (ImGui::Checkbox("Autosave", &autosave)) {
				config->setAutosave(autosave);
			}
#ifdef _DEBUG
			if (ImGui::Button("Reload Config")) {
				config->load();
			}
#endif
			ImGui::Text("Server Settings");
			if (ImGui::InputText("Port", portBuf.data(), 6, ImGuiInputTextFlags_CharsDecimal)) {
				try {
					config->setPort(std::stoi(portBuf.data()));
				}
				catch (std::invalid_argument const& e) {
				}
			}

			if (ImGui::InputText("Operations per second", opsBuf.data(), 3, ImGuiInputTextFlags_CharsDecimal)) {
				try {
					config->setOpsPerCycle(std::stoi(opsBuf.data()));
				}
				catch (std::invalid_argument const& e) {
				}
			}
			ImGui::Text("Logging");
			const char* loggingLevels[] = { "None", "Error", "Info", "Debug", "Trace" };

			int loglevel = config->getOption<int>("logger.level");
			static const char* current_item = NULL;
			LOG_LEVEL_TO_STRING(loglevel, current_item)

				if (ImGui::BeginCombo("Log level", current_item, ImGuiComboFlags_NoArrowButton))
				{
					for (int n = 0; n < IM_ARRAYSIZE(loggingLevels); n++)
					{
						bool is_selected = (current_item == loggingLevels[n]);
						if (ImGui::Selectable(loggingLevels[n], is_selected))
							current_item = loggingLevels[n];
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					loglevel = 0;

					LOG_LEVEL_FROM_STRING(loglevel, current_item)

						config->setOption("logger.level", loglevel);
					XPC::Log::SetLogLevel(loglevel);

					ImGui::EndCombo();
				}
			ImGui::EndChild();
			ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 60);

			ImGui::BeginChild("configuration_tab_btns", ImVec2(0, 0), false);
			ImGui::Separator();
			if (!config->isAutosave()) {
				if (ImGui::Button("Save")) {
					this->config->setPort(std::stoi(portBuf.data()));
					this->config->setOpsPerCycle(std::stoi(opsBuf.data()));
					this->config->save();
				}
			
			ImGui::SameLine();
			}
			if (ImGui::Button("Reload plugin")) {
				auto id = XPLMGetMyID();
				XPLMDisablePlugin(id);
				XPLMEnablePlugin(id);
			}
			ImGui::EndChild();


			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Blacklist")) {
			char buf[500] = { 0 };
			XPLMGetDatab(datarefs[XPC::DREF_AcfICAO], buf, 0, 500);
			auto acfIcao = std::string(buf);
			ImGui::TextWrapped("Use this tab to blacklist the current aircraft to prevent changing fuel state as this can break certain aircraft");
			ImGui::BeginChild("##", ImVec2(0, 0), true);
			if (!config->getTree().get_child_optional(fmt::format("blacklist.{}", acfIcao)).is_initialized()) {
				if (ImGui::Button("Add current aircraft")) {

					XPLMGetDatab(datarefs[XPC::DREF_AcfDescription], buf, 0, 500);
					auto acfDesc = std::string(buf);
					XPLMGetDatab(datarefs[XPC::DREF_AcfAuthor], buf, 0, 500);
					auto acfAuthor = std::string(buf);


					config->addToBlacklist(acfIcao, acfDesc, acfAuthor);
				}
			}
			else
			{
				if (ImGui::Button("Remove current aircraft")) {
					char buf[500] = { 0 };
					XPLMGetDatab(datarefs[XPC::DREF_AcfICAO], buf, 0, 500);
					config->removeFromBlacklist(std::string(buf));
				}
			}

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(ImColor(255, 255, 255, 20)));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			auto bl = config->getBlacklist();
			if (!bl.empty()) {
				for (auto item : bl) {
					bool fuel, payload;
					fuel = item.fuel;
					payload = item.payload;
					ImGui::BeginChild(fmt::format("blacklist_item_{}", item.icao).c_str(), ImVec2(0, 85), true);

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					ImGui::SetCursorPos(ImVec2(10, 16));
					ImGui::BeginGroup();
					ImGui::Text(fmt::format("Aircraft: {}", item.desc).c_str());
					ImGui::Text(fmt::format("Author: {}", item.author).c_str());
					ImGui::Text(fmt::format("ICAO: {}", item.icao).c_str());
					ImGui::EndGroup();
					ImGui::PopStyleVar();



					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 140, 25));
					/*
					if (ImGui::Checkbox("Fuel", &fuel)) {
						config->getTree().put<bool> (fmt::format("blacklist.{}.fuel", item.icao), fuel);
						if (config->isAutosave()) {
							config->save();
						}
					}


					ImGui::SameLine();
					if (ImGui::Checkbox("Payload", &payload)) {
						config->getTree().put<bool>(fmt::format("blacklist.{}.payload", item.icao), payload);
						if (config->isAutosave()) {
							config->save();
						}
					}
					*/
					if (ImGui::Button("Remove")) {
						config->removeFromBlacklist(item.icao);
					}

					ImGui::EndChild();
				}
				if (!config->isAutosave()) {
					if (ImGui::Button("Save")) {
						this->config->save();
					}
				}

			}

			//ImGui::SetCursorPosY(65);


			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Log")) {
			cycles++;

			if (ImGui::BeginPopup("Options")) {
				ImGui::Checkbox("Auto-scroll", &AutoScroll);
				ImGui::EndPopup();
			}
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("Options");
			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			LogFilter.Draw("Filter", -100.0f);
			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);



			if (cycles > 200) {
				auto logFile = std::ifstream("XPCLog.txt", ios::in);
				std::string line;
				if (logFile.is_open()) {
					logFile.seekg(logSeek);
					int old_size = LogBuf.size();

					while (std::getline(logFile, line)) {
						line.append("\n");
						LogBuf.append(line.c_str());
						logSeek = logFile.tellg();
					}
					for (int new_size = LogBuf.size(); old_size < new_size; old_size++)
						if (LogBuf[old_size] == '\n') {
							int ns = old_size + 1;
							logOffsets.push_back(ns);
						}
					cycles = 0;
				}
				logFile.close();
			}

			if (clear) {
				LogBuf.clear();
				logOffsets.clear();
			}
			if (copy)
				ImGui::LogToClipboard();

			const char* buf = LogBuf.begin();
			const char* buf_end = LogBuf.end();
			if (LogFilter.IsActive())
			{

				for (int line_no = 0; line_no < logOffsets.size(); line_no++)
				{
					const char* line_start = buf + logOffsets[line_no];
					const char* line_end = (line_no + 1 < logOffsets.size()) ? (buf + logOffsets[line_no + 1] - 1) : buf_end;
					if (LogFilter.PassFilter(line_start, line_end)) {
						IMLOG(line_start, line_end)
					}
				}
			}
			else {
				ImGuiListClipper clipper;
				clipper.Begin(logOffsets.size());
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + logOffsets[line_no];
						const char* line_end = (line_no + 1 < logOffsets.size()) ? (buf + logOffsets[line_no + 1] - 1) : buf_end;
						IMLOG(line_start, line_end)
					}
				}
				clipper.End();
			}




			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(2);

			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("About")) {
			ImGui::NewLine();
			ImGui::Image((void*)xpcLogo, xpclSize);


			ImGui::Text("Version: %s", XPC_PLUGIN_VERSION);
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::SameLine(ImGui::GetWindowWidth() - 90);
			ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 90);
			ImGui::Image((void*)nasaLogo, nasalSize);


			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

bool ConfigWindow::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

vector<int>& ConfigWindow::GetLogLineOffsets()
{
	return LogLineOffsets;
}
