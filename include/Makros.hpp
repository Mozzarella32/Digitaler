#pragma once

#ifdef DEFINE_MAKRO_UTILITIES
#define INNERCONCAT(a,b) a##b
#define CONCAT(a,b) INNERCONCAT(a,b)
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define TOWSTRING(x) CONCAT(L,TOSTRING(x))
#endif

#ifdef UNDEFINE_MAKRO_UTILITIES
#undef INNERCONCAT
#undef CONCAT
#undef STRINGIFY
#undef TOSTRING
#undef TOWSTRING
#endif