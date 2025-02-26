#include "pch.h"

//#define Testing
#define UseCollisionGrid


#define RenderCollisionGrid

#ifndef UseCollisionGrid
#undef RenderCollisionGrid
#endif

#define HotShaderReload

#define PROFILING
#include "Instrumentor/Instrumentor.h"