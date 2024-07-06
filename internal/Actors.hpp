#ifndef RAYTHEATER_ACTORS_H
#define RAYTHEATER_ACTORS_H 1

#ifndef RT_MAX_STAGE_ACTOR_COUNT
#define RT_MAX_STAGE_ACTOR_COUNT 0
#endif

#include "./RenderList.hpp"
#include "./types.h"
#include <cassert>
#include <unordered_set>
#include <vector>

namespace Theater {

class Stage;

class Actors {

private:
  friend class Stage;

  class Actor {
    friend class Actors;

  public:
    Actor();

  private:
    bool m_isActive;
    bool m_isAlive;
  };

public:
  typedef struct ActorTemplate {
    TheaterStageHandler *onEnter = nullptr;
    TheaterHandler *onTick = nullptr;
    TheaterDrawHandler *onStageDraw = nullptr;
    TheaterDrawHandler *onWindowDraw = nullptr;
    TheaterStageHandler *onLeave = nullptr;
    byte drawIndex = 1;
  } ActorTemplate;

  Actors();
  ActorRessource AddActor(Stage *s, ActorTemplate);
  void RemoveActor(ActorRessource);

private:
  void OnTick(Stage *, Play);
  void OnStageDraw();
  void OnWindowDraw();

  TheaterHandler m_onTick[RT_MAX_STAGE_ACTOR_COUNT];
  TheaterStageHandler m_onLeave[RT_MAX_STAGE_ACTOR_COUNT];

  RenderListNode m_renderListNodes[RT_MAX_STAGE_ACTOR_COUNT];
  RenderListNode m_renderList;

  std::unordered_set<ActorRessource> m_slots_free;
  std::vector<ActorRessource> m_slots_reclaim;

  Play *m_play;
};

// BM: Actors - Implementation
//==============================================================================
inline Actors::Actors() : m_play(nullptr) {

  m_renderList.zindex = 255;

  if (RT_MAX_STAGE_ACTOR_COUNT > 0) {
    m_slots_free.reserve(RT_MAX_STAGE_ACTOR_COUNT);
    for (int i = RT_MAX_STAGE_ACTOR_COUNT - 1; i >= 0; i--) {
      m_slots_free.insert(i);
      m_onTick[i] = NoHandler;
      m_onLeave[i] = NoStageHandler;

      m_renderListNodes[i].zindex = 0;
      m_renderListNodes[i].stageDraw = NoDrawHandler;
      m_renderListNodes[i].windowDraw = NoDrawHandler;

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

inline ActorRessource Actors::AddActor(Stage *s, ActorTemplate a) {

  auto it = m_slots_free.begin();
  assert(it != m_slots_free.end());

  ActorRessource res = *it;

  (*a.onEnter)(s);

  if (a.onTick != nullptr)
    m_onTick[res] = *(a.onTick);

  if (a.onLeave != nullptr)
    m_onLeave[res] = *(a.onLeave);

  if (a.onWindowDraw != nullptr || a.onStageDraw != nullptr) {
    m_renderListNodes[res].stageDraw = *(a.onStageDraw);
    m_renderListNodes[res].windowDraw = *(a.onWindowDraw);
    m_renderListNodes[res].zindex = a.drawIndex;
  }

  return res;
}

inline void Actors::RemoveActor(ActorRessource a) {
  assert(a < RT_MAX_STAGE_ACTOR_COUNT);
  m_slots_reclaim.push_back(a);
}

inline void Actors::OnTick(Stage *s, Play p) {

  // cleanup reclaimed actors, if needed
  if (m_slots_reclaim.size() > 0) {
    for (auto it = m_slots_reclaim.begin(); it != m_slots_reclaim.end(); it++) {

      auto slot = *it;

      // Free the slot and call onLeave on the actor, if not done yet
      if (m_slots_free.find(slot) == m_slots_free.end()) {
        m_onLeave[slot](s);
        m_slots_free.insert(slot);
      }

      // remove the slot that was freed from the renderer
      m_onLeave[slot] = NoStageHandler;
      m_onTick[slot] = NoHandler;
      m_renderListNodes[slot].stageDraw = NoDrawHandler;
      m_renderListNodes[slot].windowDraw = NoDrawHandler;
      m_renderListNodes[slot].zindex = 0;
    }
    // empty reclaim list
    m_slots_reclaim.clear();
  }

  // Tick all the actors, that are still, active
  Play play = *m_play;
  for (int i = 0; i < RT_MAX_STAGE_ACTOR_COUNT; i++)
    m_onTick[i](s, play);

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
    node->stageDraw();
}

inline void Actors::OnWindowDraw() {
  RenderListNode *node = &m_renderList;
  while ((node = node->next) != nullptr && node->zindex > 0)
    node->windowDraw();
}

} // namespace Theater

#endif