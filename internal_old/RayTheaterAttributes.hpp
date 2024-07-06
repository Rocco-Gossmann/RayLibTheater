#ifndef RAYTHEATER_ATTRIBUTES_H
#define RAYTHEATER_ATTRIBUTES_H

// Something to keep .clangd files with the -DSTAGE_ATTRIBUTE from interfering
#ifdef STAGE_ATTRIBUTE
#undef STAGE_ATTRIBUTE
#endif

namespace Theater {

// BM: Attributes - Enum
//=============================================================================
enum Attributes {

#define STAGE_ATTRIBUTE(name) name,

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

  STAGE_ATTRIBUTE(TICKING) STAGE_ATTRIBUTE(DEAD) STAGE_ATTRIBUTE(TRANSFORMABLE)
      STAGE_ATTRIBUTE(VISIBLE)

          __STAGE_ATTRIBUTE_COUNT
#undef STAGE_ATTRIBUTE
};

}
#endif
