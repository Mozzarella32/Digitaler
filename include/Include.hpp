#include "pch.hpp"

//#define Testing
#define UseCollisionGrid


#define RenderCollisionGrid

#ifndef UseCollisionGrid
#undef RenderCollisionGrid
#endif

#define HotShaderReload

#define PROFILING
#include "../submodules/Instrumentor/Instrumentor.hpp"