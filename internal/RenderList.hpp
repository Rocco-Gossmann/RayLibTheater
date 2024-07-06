#ifndef RAYTHEATER_DOUBLE_LINKED_LIST_H
#define RAYTHEATER_DOUBLE_LINKED_LIST_H 1

#include "./types.h"
#include <cassert>

namespace Theater {

typedef struct RenderListNode {

  TheaterDrawHandler stageDraw;  // maybe use tese handlers to fire an event,
  TheaterDrawHandler windowDraw; // once all actors where rendered???

  byte zindex = 0;

  RenderListNode *next = nullptr;
  RenderListNode *prev = nullptr;
  RenderListNode *root = nullptr;

} RenderList;

} // namespace Theater
#endif