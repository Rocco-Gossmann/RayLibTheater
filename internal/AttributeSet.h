#ifndef ACTOR_ATTRIBUTE
#define ACTOR_ATTRIBUTE(stuff)
#define __ACT_ATTR_DEF__
#endif

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

#ifdef __ACT_ATTR_DEF__
#undef __ACT_ATTR_DEF__
#undef ACTOR_ATTRIBUTE
#endif
