#ifndef RAYTHEATER_ACTORS_H
#define RAYTHEATER_ACTORS_H

#ifndef RT_MAX_STAGE_ACTOR_COUNT
#define RT_MAX_STAGE_ACTOR_COUNT 0
#endif

#include "./Play.h"
#include "./RenderList.hpp"
#include "./macros.h"
#include "./types.h"
#include <cassert>
#include <unordered_set>
#include <vector>

namespace Theater {

class Actors;

class Actor {
public:
  virtual void OnEnter(Stage *) {}
  virtual void OnTick(Stage *, Play) {}
  virtual void OnStageDraw() {}
  virtual void OnWindowDraw() {}
  virtual void OnLeave(Stage *) {}

  virtual AttributeList GetAttributes();
};

class Actors {

private:
  friend class Stage;

public:
  Actors();
  template <typename T>
  ActorRessource AddActor(Stage *s, T *actor, byte zIndex = 0);
  void RemoveActor(ActorRessource);

  Actor *getActorByRessource(ActorRessource);

private:
  void OnTick(Stage *, Play);
  void OnStageDraw();
  void OnWindowDraw();
  void Clear(Stage *);
  void ReclaimSlots(Stage *);

  RenderListNode m_renderListNodes[RT_MAX_STAGE_ACTOR_COUNT];
  RenderListNode m_renderList;

  std::unordered_set<ActorRessource> m_slots_free;
  std::vector<ActorRessource> m_slots_reclaim;

  Play *m_play;
};

// BM: Actor - Implementation
//==============================================================================
inline AttributeList Actor::GetAttributes() { return {}; }

// BM: Actors - Implementation
//==============================================================================
inline Actors::Actors() : m_play(nullptr) {

  m_renderList.zindex = 255;

  if (RT_MAX_STAGE_ACTOR_COUNT > 0) {
    m_slots_free.reserve(RT_MAX_STAGE_ACTOR_COUNT);
    for (int i = RT_MAX_STAGE_ACTOR_COUNT - 1; i >= 0; i--) {
      m_slots_free.insert(i);

      m_renderListNodes[i].zindex = 0;
      m_renderListNodes[i].actor = nullptr;

      if (i > 0) {
        m_renderListNodes[i].prev = &m_renderListNodes[i - 1];
        m_renderListNodes[i - 1].next = &m_renderListNodes[i];
      } else {
        m_renderListNodes[i].prev = &m_renderList;
        m_renderList.next = &m_renderListNodes[i];
      }

      if (i < RT_MAX_STAGE_ACTOR_COUNT - 1)
        m_renderListNodes[i].next = &m_renderListNodes[i + 1];
    }
  }
}

template <typename T> ActorRessource Actors::AddActor(Stage *s, T *a, byte z) {

  static_assert(std::is_base_of<Actor, T>::value,
                "given actor must be a pointer on an Theater::Actor");

  assert(RT_MAX_STAGE_ACTOR_COUNT > 0 &&
         "`#define RT_MAX_STAGE_ACTOR_COUNT` must be > 0");

  auto it = m_slots_free.begin();
  assert(it != m_slots_free.end() &&
         "actor limit reached. Increase RT_MAX_STAGE_ACTOR_COUNT, to add more");

  ActorRessource res = *it;

  a->OnEnter(s);

  m_renderListNodes[res].actor = a;
  m_renderListNodes[res].zindex = z;

  m_slots_free.erase(it);

  return res;
}

inline void Actors::RemoveActor(ActorRessource a) {
  assert(a < RT_MAX_STAGE_ACTOR_COUNT);
  m_slots_reclaim.push_back(a);
}

inline void Actors::ReclaimSlots(Stage *s) {
  if (m_slots_reclaim.size() > 0) {
    for (auto it = m_slots_reclaim.begin(); it != m_slots_reclaim.end(); it++) {

      auto slot = *it;

      // Free the slot and call onLeave on the actor, if not done yet
      if (m_slots_free.find(slot) == m_slots_free.end()) {
        m_renderListNodes[slot].actor->OnLeave(s);
        m_slots_free.insert(slot);
      }

      // remove the slot that was freed from the renderer
      m_renderListNodes[slot].actor = nullptr;
      m_renderListNodes[slot].zindex = 0;
    }
    // empty reclaim list
    m_slots_reclaim.clear();
  }
}

inline void Actors::OnTick(Stage *s, Play p) {

  // cleanup reclaimed actors, if needed
  ReclaimSlots(s);

  // Tick all the actors, that are still, active
  Play play = *m_play;
  for (int i = 0; i < RT_MAX_STAGE_ACTOR_COUNT; i++)
    if (m_renderListNodes[i].actor != nullptr)
      m_renderListNodes[i].actor->OnTick(s, play);

  // Reorder RenderList if needed
  for (int i = 1; i < RT_MAX_STAGE_ACTOR_COUNT; i++) {
    auto node = &m_renderListNodes[i];
    while (node->prev != nullptr && node->prev->zindex < node->zindex) {
      auto prev = node->prev;

      if (node->next != nullptr) {
        node->next->prev = prev;
        prev->next = node->next;
      } else {
        prev->next = nullptr;
      }

      if (prev->prev != nullptr) {
        prev->prev->next = node;
        node->prev = prev->prev;
      } else {
        node->prev = nullptr;
      }

      node->next = prev;
      prev->prev = node;
    }
  }
}

inline void Actors::OnStageDraw() {
  RenderListNode *node = &m_renderList;
  while ((node = node->next) != nullptr && node->zindex > 0)
    node->actor->OnStageDraw();
}

inline void Actors::OnWindowDraw() {
  RenderListNode *node = &m_renderList;
  while ((node = node->next) != nullptr && node->zindex > 0)
    node->actor->OnWindowDraw();
}

inline void Actors::Clear(Stage *s) {

  ReclaimSlots(s);

  for (int i = 0; i < RT_MAX_STAGE_ACTOR_COUNT; i++) {
    if (m_renderListNodes[i].actor != nullptr) {

      DebugLog("[Actors::Clear] found still registered Actor " << i);

      m_renderListNodes[i].actor->OnLeave(nullptr);
      m_renderListNodes[i].actor = nullptr;
      m_renderListNodes[i].zindex = 0;
    }
  }
}

inline Actor *Actors::getActorByRessource(ActorRessource r) {
  assert(r < RT_MAX_STAGE_ACTOR_COUNT &&
         "requested actor is not part of the pool");

  return m_renderListNodes[r].actor;
};

} // namespace Theater

#endif
