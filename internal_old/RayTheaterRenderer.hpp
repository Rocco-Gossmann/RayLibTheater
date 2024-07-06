#ifndef RAYTHEATER_ATTRIBUTES_H
#define RAYTHEATER_ATTRIBUTES_H

#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Theater {

// Defining how many actors can be handled each frame
#ifndef ACTORLIMIT
#define ACTORLIMIT 64
#endif

class Visible;
typedef unsigned char byte;

class Renderer {
private:
  class RenderNode {
    friend class Renderer;

  public:
    void SetZIndex(byte zindex);

  private:
    RenderNode();
    void SetObject(Visible *obj);

    Visible *m_obj;
    byte m_zindex;
    unsigned int m_index;
  };

public:
  Renderer();
  RenderNode *RegisterVisible(Visible *);
  void UnregisterVisible(RenderNode *);

private:
  RenderNode m_slots[ACTORLIMIT];
  std::vector<unsigned int> m_freeSlots;
  std::unordered_set<unsigned int> m_usedSlots;
};

inline Renderer::Renderer() {
  for (int i = 0; i < ACTORLIMIT; i++) {
    m_freeSlots.push_back(i);
    m_slots[i].m_index = i;
  }
}

inline Renderer::RenderNode *Renderer::RegisterVisible(Visible *obj) {
  assert(obj != nullptr);
  assert(m_freeSlots.size() > 0);

  unsigned int freeSlot = this->m_freeSlots.back();
  m_slots[freeSlot].m_zindex = 0;
  m_slots[freeSlot].m_obj = obj;

  m_freeSlots.pop_back();
  m_usedSlots.insert(freeSlot);

  return &m_slots[freeSlot];
}

inline void Renderer::UnregisterVisible(RenderNode *node) {
  assert(node != nullptr);
  const auto idx = node->m_index;
  auto it = m_usedSlots.find(idx);
  assert(it != m_usedSlots.end());
}
// BM: RenderNode - Implement
//==============================================================================
inline Renderer::RenderNode::RenderNode()
    : m_obj(nullptr), m_zindex(0), m_index(0) {}

inline void Renderer::RenderNode::SetZIndex(byte zindex) { m_zindex = zindex; }

// Defining how many actors can be handled each frame

// BM: RenderNode - Struct
//==============================================================================
template <typename T> struct RenderNode {
  int index;
  T *obj;
  bool alive;
  RenderNode<T> *next;
  RenderNode<T> *prev;
};

const RenderNode<Visible> _renderNodes[ACTORLIMIT + 1];
unsigned int _renderNodeCnt = 0;
RenderNode<Visible> _renderNodeList;

void Render() {

  // Figure out the Render order of actors;
  int rnCnt = _renderNodeCnt;
  RenderNode<Visible> *rn = &_renderNodeList;
  rn->next = NULL;

  for (int a = 0; a < rnCnt; a++) {
    auto node = &(_renderNodes[a]);
    node->index = node->obj->_zindex;

    if (node->alive == false)
      continue;

    if (node->index >= rn->index) {
      while (rn->next != NULL && rn->next->index <= node->index)
        rn = rn->next;

      node->next = NULL;
      if (rn->next != NULL) {
        rn->next->prev = node;
        node->next = rn->next;
      }

      rn->next = node;
      node->prev = rn;

    } else if (node->index < rn->index) {
      while (rn->prev != NULL && rn->prev->index > node->index)
        rn = rn->prev;

      rn->prev->next = node;
      node->prev = rn->prev;
      node->next = rn;
      rn->prev = node;
    }

    rn = node;
  }
}

} // namespace Theater
#endif
