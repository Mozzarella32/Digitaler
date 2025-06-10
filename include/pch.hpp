#pragma once

#include <glad/glad.h>

#ifdef _WIN32

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
#include <GL/gl.h>
#include <Windows.h>
#endif
#include <GL/glu.h>

#include <GLInitialiser.hpp>
#include <Utility.hpp>
// #include <MyWxWidgets/ElementLayerer.h>
#include <GameLoopTimer.hpp>

#include <Shader.hpp>
#include <Texture.hpp>
#include <Utilities.hpp>
#include <VertexBuffer.hpp>

#include "../submodules/Visitor/Visitor.hpp"

#include "../submodules/LinearAlgebra/Point.hpp"

#include <wx/artprov.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/gbsizer.h>
#include <wx/hyperlink.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <wx/popupwin.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>
#include <wx/treectrl.h>
#include <wx/wfstream.h>
#include <wx/wrapsizer.h>
#include <wx/wx.h>
#include <wx/zipstrm.h>
#include <wx/glcanvas.h>

#include <array>
#include <queue>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <strstream>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <wx/treelist.h>

#include <numbers>

#include <ranges>

// #include <nlohmann/json.hpp>

#include "Eigen/Dense"
////
// #define MSDFGEN_PUBLIC
////
// #include <msdfgen/core/base.h>
//
// #include <msdf-atlas-gen/msdf-atlas-gen.h>
