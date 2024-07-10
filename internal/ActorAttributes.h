#ifndef RAYTHEATER_ACTOR_ATTRIBUTES_H
#define RAYTHEATER_ACTOR_ATTRIBUTES_H

namespace Theater {

enum ATTRIBUTES {

#define ACTOR_ATTRIBUTE(attr) attr,
#include "./internal/AttributeSet.h"
#undef ACTOR_ATTRIBUTE

  __ATTRIBUTE_COUNT
};

} // namespace Theater

#endif
