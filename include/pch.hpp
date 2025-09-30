#pragma once

#include <glad/glad.h>

#ifdef _WIN32

#undef APIENTRY

#ifndef _SILENCE_CXX23_DENORM_DEPRECATION_WARNING
#define _SILENCE_CXX23_DENORM_DEPRECATION_WARNING
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define _CRT_SECURE_NO_WARNINGS

//#pragma warning(push)
//#pragma warning(disable : 4005)

#include <winsock2.h>
#include <Windows.h>

//#pragma warning(pop)
#endif

 //#ifdef _WIN32
 //#include <GL/gl.h>
 //#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/mstream.h>
#include <wx/menu.h>
#include <wx/slider.h>
#include <wx/panel.h>

#include <GLInitialiser.hpp>
#include <Utility.hpp>
#include <GameLoopTimer.hpp>

#include <Shader.hpp>
#include <Texture.hpp>
#include <Utilities.hpp>
#include <VertexBuffer.hpp>
#include <WxTextureUtil.hpp>

#include "../submodules/MyWxWidgets/submodules/MyOpenGl/submodules/Visitor/Visitor.hpp"

#include "../submodules/LinearAlgebra/Point.hpp"

#include <array>
#include <queue>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <chrono>

#include <numbers>
#include <ranges>
#include <functional>

// #include <nlohmann/json.hpp>

#include "Eigen/Dense"
////
// #define MSDFGEN_PUBLIC
////
// #include <msdfgen/core/base.h>
//
// #include <msdf-atlas-gen/msdf-atlas-gen.h>
