#include "pch.hpp"

//#define Testing
#define UseCollisionGrid

#define AlwaysDraw

// #define RenderCollisionGrid
// #define ShowBasePositionOfBlocks
// #define ShowBoundingBoxes

#ifndef UseCollisionGrid
#undef RenderCollisionGrid
#endif


#define HotShaderReload

#define PROFILING
#include "../submodules/Instrumentor/Instrumentor.hpp"
