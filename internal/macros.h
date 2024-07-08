#ifndef MACROS_H
#define MACROS_H

#include <raylib.h>

#ifdef DEBUG
#include <iostream>
#define DebugLog(msg) std::cout << "[DebugLog] " << msg << std::endl
#else
#define DebugLog(msg)
#endif

#ifdef TESTS
#define TestOnly(stuff) stuff;
#else
#define TestOnly(stuff) /**/ ;
#endif

#endif
