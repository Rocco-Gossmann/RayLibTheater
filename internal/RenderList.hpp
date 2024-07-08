#ifndef RAYTHEATER_DOUBLE_LINKED_LIST_H
#define RAYTHEATER_DOUBLE_LINKED_LIST_H 1

#include "./types.h"
#include <cassert>

namespace Theater {

class Actor;

typedef struct RenderListNode {

  Actor *actor = nullptr;

  byte zindex = 0;

  RenderListNode *next = nullptr;
  RenderListNode *prev = nullptr;
  RenderListNode *root = nullptr;

} RenderList;

} // namespace Theater
#endif
