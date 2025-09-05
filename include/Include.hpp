#include "pch.hpp"

//#define Testing
#define UseCollisionGrid


//Not working
#define RenderCollisionGrid

#ifndef UseCollisionGrid
#undef RenderCollisionGrid
#endif

#define ShowBasePositionOfBlocks
#define ShowBoundingBoxes

#define HotShaderReload

#define PROFILING
#include "../submodules/Instrumentor/Instrumentor.hpp"
