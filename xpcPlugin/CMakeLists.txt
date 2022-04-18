cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(XPlaneConnect)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SDK/CHeaders/XPLM)
include_directories(lib/GL)
include_directories(lib/KHR)
include_directories(imgui)
include_directories(imgui/misc/freetype)
include_directories(xsb_public)
include_directories(../C/src)

add_definitions(-DXPLM200 -DXPLM300 -DXPLM301 -DXPLM210 -DXPLM_DEPRECATED -DLIN=1)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_STANDARD 20)

SET(XPC_OUTPUT_DIR "XPlaneConnect")
SET(XPC_OUTPUT_NAME "lin")

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})

add_library(xpc64 SHARED 
	Imgui/imgui.cpp
	Imgui/imguo_draw.cpp
	Imgui/imgui_freetype.cpp
	Imgui/imgui_tables.cpp
	Imgui/imgui_widgets.cpp
	xsb_public/ImgWindow.cpp
	xsb_public/ImgFreeTypeFontAtlas.cpp
	xsb_public/XOGLUtils.cpp
	xsb_public/WavFile.cpp
	XPCPlugin.cpp
	CameraCallbacks.cpp
	DataManager.cpp
	Drawing.cpp
	Log.cpp
	Configuration.cpp
	ConfigWindow.cpp
	Message.cpp
	MessageHandlers.cpp
	Timer.cpp
	UDPSocket.cpp)
set_target_properties(xpc64 PROPERTIES PREFIX "" SUFFIX ".xpl")
set_target_properties(xpc64 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${XPC_OUTPUT_DIR}/64)
set_target_properties(xpc64 PROPERTIES OUTPUT_NAME ${XPC_OUTPUT_NAME})
set_target_properties(xpc64 PROPERTIES COMPILE_FLAGS "-m64 -fno-stack-protector" LINK_FLAGS "-shared -rdynamic -nodefaultlibs -undefined_warning -m64 -fno-stack-protector")
target_link_libraries(xpc64 ${Boost_LIBRARIES})

install(TARGETS xpc64 DESTINATION XPlaneConnect/64 RENAME lin.xpl)