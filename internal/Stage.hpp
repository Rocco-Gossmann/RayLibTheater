#ifndef RAYTHEATER_STAGE_H
#define RAYTHEATER_STAGE_H

#include "raylib.h"
#include <cassert>

#include "./macros.h"
#include "./types.h"

#include "./Actors.hpp"

namespace Theater {

class Builder;

class Stage {

private:
  friend class Builder;

  enum StageProcess { PAUSED, UPDATE, DRAW, WINDOWDRAW, SWAPSCENE, SHUTDOWN };

public:
  //==============================================================================
  // BM: Stage::Scene - Class
  //==============================================================================
  class Scene {
  public:
    virtual void OnLoad(Stage *s){};
    virtual void OnTick(Stage *s, Play p){};
    virtual void OnSceneDrawBG(){};
    virtual void OnSceneDrawFG(){};
    virtual void OnWindowDraw(){};
    virtual void OnUnload(Stage *s){};
  };

  Stage();
  template <typename T> const void ChangeScene(T *s = nullptr);
  const void EndPlay();

  /** @brief Adds an Actor to the stage
   * @param actor - a pointer to an instance that inherits from "Theater::Actor"
   * @param zIndex - =0 == actor will be processed, but not rendered
                     >0 == actor will be processed and rendered the bigger the
                           number, the further back the actor will be moved on
                           the stage (aka. the more elements can appear above
                           it)
  */
  template <typename A> ActorRessource AddActor(A *actor, byte zIndex = 0);
  void RemoveActor(ActorRessource);

  std::unordered_set<Actor *> getActorsWithAttribute(ATTRIBUTES);

private:
  StageProcess m_stageProcess;
  Play m_play;
  Rectangle m_viewportrect;
  Rectangle m_stagerect;
  RenderTexture m_stagetexture;
  bool m_integerScale;
  Color m_borderColor;
  Color m_backgroundColor;
  float m_scale;

  Actors m_actors;

  Scene *m_currentStageState;
  Scene *m_nextStageState;

  /** defines if the stages state has been changes this cycle */
  bool m_stageStateSet;

  bool swapStageStates();
  void shutdown();
  void onResize();
  void updatePlay();

  void play();

  // set lists of actors with certain
  std::unordered_map<ATTRIBUTES, ActorRessourceList> m_attributeLists;
};

// BM: Implementation
//==============================================================================

inline Stage::Stage()
    : m_currentStageState(nullptr), m_nextStageState(nullptr),
      m_stageProcess(PAUSED), m_actors(), m_viewportrect({0, 0, 256, 192}),
      m_integerScale(false), m_stagerect({0, 0, 256, 192}), m_stagetexture(),
      m_play(), m_attributeLists({}) {
  m_actors.m_play = &m_play;

#define ACTOR_ATTRIBUTE(attr)                                                  \
  m_attributeLists.insert({attr, ActorRessourceList{}});
#include "./internal/AttributeSet.h"
#undef ACTOR_ATTRIBUTE
}

template <typename T> inline const void Stage::ChangeScene(T *s) {
  static_assert(std::is_base_of<Stage::Scene, T>::value,
                "Given Scene must be a child of class Stage::Scene");
  m_nextStageState = (Stage::Scene *)s;
  m_stageStateSet = true;
}

inline const void Stage::EndPlay() {
  DebugLog("[Stage::EndPlay] invoced " << m_nextStageState);
  m_nextStageState = nullptr;
  m_stageStateSet = true;
}

template <typename A> ActorRessource Stage::AddActor(A *a, byte z) {
  auto res = m_actors.AddActor(this, a, z);

  for (auto attr : ((Actor *)a)->GetAttributes()) {
    const auto it = m_attributeLists.find(attr);
    if (it != m_attributeLists.end())
      it->second.insert(res);
  }

  return res;
}

inline void Stage::RemoveActor(ActorRessource a) {
  m_actors.RemoveActor(a);

  auto it = m_attributeLists.begin();
  while (it != m_attributeLists.end()) {
    it->second.erase(a);
    it++;
  }
}

inline bool Stage::swapStageStates() {
  if (m_stageStateSet) {
    DebugLog("[Stage::swapStageStages] invoced " << m_nextStageState);

    const auto osp = m_stageProcess;
    m_stageProcess = SWAPSCENE;

    if (m_currentStageState != nullptr) {
      DebugLog("[Stage::swapStageStages] current scene unload "
               << m_currentStageState);
      m_currentStageState->OnUnload(this);
    }

    m_currentStageState = m_nextStageState;
    m_nextStageState = nullptr;
    m_stageStateSet = false;

    if (m_currentStageState != nullptr) {
      DebugLog("[Stage::swapStageStages] next scene load "
               << m_currentStageState);
      m_currentStageState->OnLoad(this);
    }

    m_stageProcess = osp;

    return true;
  }

  return false;
}

inline void Stage::shutdown() {
  DebugLog("[Stage::shutdown] invoked");

  EndPlay();
  swapStageStates();

  m_actors.Clear(this);

  UnloadRenderTexture(m_stagetexture);
  CloseWindow();

  m_attributeLists.clear();
}

inline void Stage::onResize() {
  float screenWidth = GetScreenWidth();
  float screenHeight = GetScreenHeight();
  float scale = ::std::min(screenWidth / m_stagerect.width,
                           screenHeight / m_stagerect.height);

  if (m_integerScale) {
    scale = ::std::max(1.0, (double)floor(scale));
  }

  m_scale = scale;
  m_play.windowWidth = screenWidth;
  m_play.windowHeight = screenWidth;
  m_play.stageScale = scale;

  m_viewportrect.width = m_stagerect.width * scale;
  m_viewportrect.height = m_stagerect.height * scale;
  m_viewportrect.x = (screenWidth - m_viewportrect.width) * 0.5;
  m_viewportrect.y = (screenHeight - m_viewportrect.height) * 0.5;
}

inline void Stage::updatePlay() {
  DebugLog("[Stage::updatePlay] invoked");
  // Put DeltaTime - Multiplyer into context
  m_play.deltaTime = GetFrameTime();

  // Update MousePosition
  m_play.mouseLoc = Vector2({(float)GetMouseX(), (float)GetMouseY()});

  m_play.mouseLoc.x -= m_viewportrect.x;
  m_play.mouseLoc.y -= m_viewportrect.y;

  m_play.mouseLoc.x /= m_scale;
  m_play.mouseLoc.y /= m_scale;

  m_play.mouseX = std::floor(m_play.mouseLoc.x);
  m_play.mouseY = std::floor(m_play.mouseLoc.y);

  // Update MouseButtons
  m_play.mouseReleased = m_play.mouseHeld | m_play.mouseDown;
  m_play.mouseHeld = 0;
  m_play.mouseUp = 0;
  m_play.mouseDown = 0;

  for (unsigned char a = 1; a < 7; a++) {
    m_play.mouseDown |= (IsMouseButtonPressed(a - 1) ? 1 : 0) << a;
    m_play.mouseHeld |= (IsMouseButtonDown(a - 1) ? 1 : 0) << a;
    m_play.mouseUp |= (IsMouseButtonUp(a - 1) ? 1 : 0) << a;
  }

  // Just in case held and Pressed overlap => remove Pressed from held.
  m_play.mouseHeld &= ~m_play.mouseDown;
  m_play.mouseUp &= ~(m_play.mouseDown | m_play.mouseHeld);
  m_play.mouseReleased &= m_play.mouseUp;
}

inline std::unordered_set<Actor *> Stage::getActorsWithAttribute(ATTRIBUTES a) {
  std::unordered_set<Actor *> ret;

  const auto it = m_attributeLists.find(a);
  if (it != m_attributeLists.end()) {
    for (auto i : (*it).second) {
      ret.insert(m_actors.getActorByRessource(i));
    }
  }

  return ret;
}

// BM: Implementation - Play
//==============================================================================

inline void Stage::play() {

  swapStageStates();

  while (m_currentStageState != nullptr && !WindowShouldClose()) {

    DebugLog("[Stage::play] Tick");

    // switch to a new scene if needed.
    if (swapStageStates())
      continue;

    // React to Window being resized
    if (IsWindowResized())
      onResize();

    // TODO: Update Play
    updatePlay();

    // Tick the stage
    DebugLog("[Stage::play] call Scene->OnTick ");
    m_currentStageState->OnTick(this, m_play);

    // Tick the actors
    DebugLog("[Stage::play] call Actors->OnTick ");
    m_actors.OnTick(this, m_play);

    // Draw the Stage
    BeginTextureMode(m_stagetexture);
    ClearBackground(m_backgroundColor);
    DebugLog("[Stage::play] call Scene->OnSceneDrawBG ");
    m_currentStageState->OnSceneDrawBG();
    DebugLog("[Stage::play] call Actors->OnStageDraw ");
    m_actors.OnStageDraw();
    DebugLog("[Stage::play] call Scene->OnSceneDrawFG ");
    m_currentStageState->OnSceneDrawFG();
    EndTextureMode();

    // Render the Stage
    BeginDrawing();
    ClearBackground(m_borderColor);
    DrawTexturePro(m_stagetexture.texture, m_stagerect, m_viewportrect, {0, 0},
                   0, WHITE);
    DebugLog("[Stage::play] call Scene->OnWindowDraw ");
    m_currentStageState->OnWindowDraw();
    EndDrawing();
  }

  shutdown();
}

} // namespace Theater
#endif
