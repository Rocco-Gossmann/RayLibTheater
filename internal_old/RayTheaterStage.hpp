#ifndef RAYTHEATER_H
#define RAYTHEATER_H 1

#include <raylib.h>

namespace Theater {

class Builder;

class Stage {
  friend class Builder;

public:
  ~Stage() { _scene = NULL; }

  /** @brief Changes the Color of the Border, that is show, when the window is
   * scaled to an aspect ratio different, than the Stages
   */
  void BorderColor(Color);

  /** @brief Background-Color of the stage. (Each frame, the stage is cleared
   * to this color.
   */
  void BackgroundColor(Color);

  /** @brief Registers an Actor as being on the stage
   *
   * @param T any class that implement Theater::Actor
   * @param a - reference that the Stage will use to adress the Actor
   */
  template <typename T> void AddActor(T *a);

  /** @brief Marks any Actor as "DEAD", meaning, it will be removed from the
   * stage between cycles
   *
   * @param a the pointer to the actor to remove (Must be the same value as
   * given in AddActor)
   */
  template <typename T> void RemoveActor(T *a);

  /** @brief Pauses all Ticking Actors */
  void Pause();

  /**  @brief Continues to run all Ticking Actors */
  void UnPause();

  /** @brief By default Actors are invisible / Not rendered
   * use this function to make it visible (Add it to the Stages render-list)
   *
   * @tparam T any class that implements Theater::Actor and Theater::Visible
   * @return true on success ; false == to many Actors are already on stage or
   * you called this function during rendering
   */
  template <typename T> bool MakeActorVisible(T *);

  /** @brief removes any visible Actor from the stages render-list
   *
   * @tparam T any class that implements Theater::Visible
   */
  template <typename T> void MakeActorInvisible(T *);

  /**
   * @brief Allows for adding custom Attributes to an Actor on the Stage
   *
   * @return - true = attribute added; false = Actor not on stage or used
   * illegal Attribute
   */
  bool AddActorAttribute(Actor *, Attributes);

  /**
   * @brief Removes a custom Attribute from an Actor on the Stage
   *
   * @return - true = attribute removed; false = Actor not on stage or used
   illegal
   * Attribute

   */
  bool RemoveActorAttribute(Actor *, Attributes);

  /**
   * @brief gets an unordered_set of Actors all having the requested Attributes
   *
   * @param attr
   */
  std::unordered_set<Actor *> GetActorsWithAttribute(Attributes attr);

  /** @brief adds a running timer to the stage
   * @param t - the timer to add
   */
  void SetTimer(Timer *t);

  /** @brief used to clean up timers, that were added to the stage
   * timers that don't loop are removed automatically if they run out.
   * @param t - the timer to add
   */
  void RemoveTimer(Timer *t);

private:
  Stage(int width, int height, float scale = 1.0);

  RenderNode<Visible> _renderNodes[ACTORLIMIT];
  unsigned int _renderNodeCnt;
  RenderNode<Visible> _renderNodeRoot;

  const char *_stageTitle;
  float _stageWidth;
  float _stageHeight;
  float _stageScale;
  bool _integerScale;

  RenderTexture2D _stage;
  Scene *_scene;
  Rectangle _viewportRect;
  Rectangle _stageRect;
  Vector2 _viewportOrigin;

  Color _borderColor;
  Color _backgroundColor;

  Theater::Play _play;

  bool _rendering;
  bool _sceneUnloading;
  bool _tickingPaused;

  std::unordered_set<Actor *> _actorsToClear;
  std::unordered_set<Ticking *> _handle_TICKING;
  std::unordered_set<Transform2D *> _handle_TRANSFORMABLE;
  std::unordered_set<Actor *> _handle_DEAD;
  std::unordered_set<Timer *> _handle_TIMER;

#define STAGE_ATTRIBUTE(name) std::unordered_set<Actor *> _handle_##name;
  STAGE_ATTRIBUTE(VISIBLE);
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")
#undef STAGE_ATTRIBUTE

  void Play(Scene *sc);

  void switchScene(Scene *);
  void onResize();

  template <typename T> void ClearActorFromStage(T *a);
  void ClearStage();
};

// BM: Implementation
//==============================================================================

inline Stage::Stage(int width, int height, float scale)
    : _scene(0x0), _viewportOrigin({0, 0}),
      _stageRect(
          {0, 0, static_cast<float>(width), static_cast<float>(-height)}),
      _viewportRect({0, 0, static_cast<float>(width) * scale,
                     static_cast<float>(height) * scale}),
      _stageWidth(width), _stageHeight(height), _play(),
      _backgroundColor(Color{0x00, 0x00, 0xAA, 0xff}),
      _borderColor(Color{0x00, 0x88, 0xff, 0xff}), _handle_TICKING(),
      _handle_DEAD(), _handle_TRANSFORMABLE(), _stageScale(scale),
      _actorsToClear(), _stageTitle("< RayWrapC - Project >"), _renderNodes(),
      _rendering(false), _tickingPaused(false), _integerScale(false) {

  static_assert(ACTORLIMIT > 0, "`#define ACTORLIMIT` must be bigger than 0");

  // Attributes Initialized
  //----------------------------------------------------------------------------
#define STAGE_ATTRIBUTE(name) _handle_##name = std::unordered_set<Actor *>();
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
  STAGE_ATTRIBUTE(VISIBLE);
#endif // __has_include("RayTheaterAttributes.hpp")

#undef STAGE_ATTRIBUTE

  ClearStage();
  _renderNodeRoot.next = NULL;
  _renderNodeRoot.prev = NULL;
  _renderNodeRoot.obj = NULL;
  _renderNodeRoot.alive = false;
  _renderNodeRoot.index = INT_MIN;
}

// BM: Stage - Implementation - Play
//------------------------------------------------------------------------------
inline void Stage::Play(Scene *sc) {

  // Setup the Window
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(_viewportRect.width, _viewportRect.height, _stageTitle);
  if (!IsWindowReady()) {
    throw "window not ready";
  }

  // Setup the Render-Area / Stage-Texture
  _stage = LoadRenderTexture(_stageWidth, _stageHeight);
  if (!IsRenderTextureReady(_stage)) {
    CloseWindow();
    throw "failed to create render texture";
  }

  // Prepare the _play - context
  _play.stage = this;
  _play.stageWidth = _stageWidth;
  _play.stageHeight = _stageHeight;
  _play.stageScale = _stageScale;
  _play.windowWidth = _viewportRect.width;
  _play.windowHeight = _viewportRect.height;

  // Activate the given scene
  switchScene(sc);

  // Start the main-Loop
  while (!WindowShouldClose() && _scene != NULL) {

    // Tick the Scene with the state crated by the last frame.
    if (!_scene->Tick(_play)) {
      // If the scene should end, attempt a scene Switch instead of
      // continuing.
      switchScene(NULL);
      continue;
    }

    // Remove all Actors, that have been killed in the last cycle.
    for (auto dead : _handle_DEAD)
      ClearActorFromStage(dead);

    // Flip all the Actors State
    for (auto act : _handle_TRANSFORMABLE)
      act->FlipTransform2DStates();

    // React to Window being resized
    if (IsWindowResized())
      onResize();

    // Put DeltaTime - Multiplyer into context
    _play.deltaTime = GetFrameTime();

    // Update MousePosition
    _play.mouseLoc = Vector2({(float)GetMouseX(), (float)GetMouseY()});

    _play.mouseLoc.x -= _viewportRect.x;
    _play.mouseLoc.y -= _viewportRect.y;

    _play.mouseLoc.x /= _stageScale;
    _play.mouseLoc.y /= _stageScale;

    _play.mouseX = std::floor(_play.mouseLoc.x);
    _play.mouseY = std::floor(_play.mouseLoc.y);

    // Update MouseButtons
    _play.mouseReleased = _play.mouseHeld | _play.mouseDown;
    _play.mouseHeld = 0;
    _play.mouseUp = 0;
    _play.mouseDown = 0;

    for (unsigned char a = 1; a < 7; a++) {
      _play.mouseDown |= (IsMouseButtonPressed(a - 1) ? 1 : 0) << a;
      _play.mouseHeld |= (IsMouseButtonDown(a - 1) ? 1 : 0) << a;
      _play.mouseUp |= (IsMouseButtonUp(a - 1) ? 1 : 0) << a;
    }

    // Just in case held and Pressed overlap => remove Pressed from held.
    _play.mouseHeld &= ~_play.mouseDown;
    _play.mouseUp &= ~(_play.mouseDown | _play.mouseHeld);
    _play.mouseReleased &= _play.mouseUp;

    // Update Timers
    std::vector<Timer *> cleanupTimers;
    for (auto t : _handle_TIMER) {
      if (!t->stageUpdate(_play))
        cleanupTimers.push_back(t);
    }

    for (auto t : cleanupTimers)
      _handle_TIMER.erase(t);

    cleanupTimers.clear();

    // Tick all the actors
    if (!_tickingPaused)
      for (Ticking *ticker : _handle_TICKING)
        ticker->OnTick(_play);

    _rendering = true;

    // Start drawing on the Stage
    BeginTextureMode(_stage);
    ClearBackground(_backgroundColor);

    rn = _renderNodeRoot.next;
    while (rn != NULL) {
      rn->obj->OnDraw(_play);
      rn = rn->next;
    }

    _scene->OnStageDraw(_play);
    EndTextureMode();

    // Start drawing on the Stage
    BeginDrawing();
    ClearBackground(_borderColor);
    DrawTexturePro(_stage.texture, _stageRect, _viewportRect, _viewportOrigin,
                   0, WHITE);
    _scene->OnWindowDraw(_play);
    EndDrawing();

    _rendering = false;
  }

  switchScene(0);
  ClearStage();
  UnloadRenderTexture(_stage);
}

inline void Stage::Pause() { _tickingPaused = true; }
inline void Stage::UnPause() { _tickingPaused = false; }

inline void Stage::switchScene(Scene *sc) {
  _sceneUnloading = true;

  if (_scene != NULL) {

    if (sc == NULL)
      sc = _scene->Unload(_play);
    else
      _scene->Unload(_play);

    _scene = NULL;
  }

  if (sc != NULL) {
    sc->OnStart(_play);
    _scene = sc;
  }

  _sceneUnloading = false;
}

inline void Stage::BorderColor(Color c) { _borderColor = c; }

inline void Stage::BackgroundColor(Color c) { _backgroundColor = c; }

inline void Stage::onResize() {
  float screenWidth = GetScreenWidth();
  float screenHeight = GetScreenHeight();
  float scale =
      ::std::min(screenWidth / _stageWidth, screenHeight / _stageHeight);

  if (_integerScale) {
    scale = ::std::max(1.0, (double)floor(scale));
  }

  _stageScale = scale;
  _play.windowWidth = screenWidth;
  _play.windowHeight = screenWidth;
  _play.stageScale = scale;

  _viewportRect.width = _stageWidth * scale;
  _viewportRect.height = _stageHeight * scale;
  _viewportRect.x = (screenWidth - _viewportRect.width) * 0.5;
  _viewportRect.y = (screenHeight - _viewportRect.height) * 0.5;
}

template <typename T> inline void Stage::AddActor(T *a) {
  static_assert(std::is_base_of<Actor, T>::value,
                "Can't add class, that does not inherit from Theater::Actor");

  _actorsToClear.insert(a);

  if (std::is_base_of<Ticking, T>::value)
    _handle_TICKING.insert((Ticking *)(a));

  if (std::is_base_of<Transform2D, T>::value)
    _handle_TRANSFORMABLE.insert((Transform2D *)(a));

  ((Actor *)a)->OnStageEnter(_play);
}

/** @brief Will remove an Actor from the stage, on the start of the next Cycle
 *
 * @param a - the actor to remove
 */
template <typename T> inline void Stage::RemoveActor(T *a) {
  static_assert(std::is_base_of<Actor, T>::value,
                "Cant call RemoveActor with none Actors");

  if (_sceneUnloading)
    ClearActorFromStage(a);
  else {
    a->_attributes.insert(DEAD);
    _handle_DEAD.insert(a);
  }
}

/** @brief Activates the Rendinger of the actor on the stage
 *
 * Can only execute, when not rendering.
 *
 * @tparam T  - Any Class extending Theater::Actor and Theater::Visible
 * @param actor - an instance of the A Class extending Theater::Actor and
 * Theater::Visible
 * @return true = Actor is now visible ; false = limit of visible actors
 * reached or you tried to change an actors visibility during rendering.
 */
template <typename T> inline bool Stage::MakeActorVisible(T *actor) {
  if (this->_rendering)
    return false;

  // Make sure the given Object supports the right classes
  static_assert(
      std::is_base_of<Actor, T>::value,
      "Can't make a class visible, that does not implement Theater::Actor");

  static_assert(
      std::is_base_of<Visible, T>::value,
      "Can't make a class visible, that does not implement Theater::Visible");

  // Should there be no free slots anymore, do nothing and fail
  if (_renderNodeCnt == ACTORLIMIT)
    return false;

  // Otherwise set the splot tho the new object
  _renderNodes[_renderNodeCnt].obj = (Visible *)actor;
  _renderNodes[_renderNodeCnt].alive = true;
  _renderNodes[_renderNodeCnt].next = NULL;
  _renderNodes[_renderNodeCnt].prev = NULL;
  ((Visible *)actor)->_renderListIndex = _renderNodeCnt;
  _renderNodeCnt++;

  // Give the Actor the "Visible" Attribute
  ((Actor *)actor)->_attributes.insert(VISIBLE);

  // Return successfully
  return true;
}

template <typename T> inline void Stage::MakeActorInvisible(T *actor) {
  if (this->_rendering)
    return;

  // make sure the given Object has the right Classes
  static_assert(
      std::is_base_of<Visible, T>::value,
      "Can't make a class visible, that does not implement Theater::Visible");

  // If there is no Objects to remove, do nothing
  if (_renderNodeCnt == 0)
    return;

  // If there is no Objects to remove, do nothing
  Visible *vis = (Visible *)actor;
  if (std::is_base_of<Actor, T>::value) {
    ((Actor *)actor)->_attributes.erase(VISIBLE);
  }

  // if the objects _renderListIndex is -1 -> do nothing
  if (vis->_renderListIndex == -1)
    return;

  _renderNodeCnt--;
  // If the removed element is not the last one.
  if (vis->_renderListIndex != _renderNodeCnt) {
    // Move the last elements content to the slot of the element to remove
    _renderNodes[vis->_renderListIndex].obj = _renderNodes[_renderNodeCnt].obj;

    _renderNodes[vis->_renderListIndex].alive =
        _renderNodes[_renderNodeCnt].alive;
    _renderNodes[vis->_renderListIndex].next =
        _renderNodes[_renderNodeCnt].next;
    _renderNodes[vis->_renderListIndex].prev =
        _renderNodes[_renderNodeCnt].prev;

    _renderNodes[vis->_renderListIndex].obj->_renderListIndex =
        vis->_renderListIndex;
  }

  // Then remove the last Slot
  _renderNodes[_renderNodeCnt].obj = NULL;
  _renderNodes[_renderNodeCnt].alive = false;
  _renderNodes[_renderNodeCnt].next = NULL;
  _renderNodes[_renderNodeCnt].prev = NULL;
}

inline bool Stage::AddActorAttribute(Actor *act, Attributes attr) {
  if (_actorsToClear.find(act) == _actorsToClear.end()) {
    return false;
  }

  switch (attr) {
  case DEAD:
    std::cout << " Can't assign ActorAttribute 'DEAD' via "
                 "Stage::AddActorAttribute => use Stage::RemoveActor instead"
              << std::endl;
    return false;

  case TICKING:
    std::cout
        << " Can't assign Internal Attribute 'TICKING' via AddActorAttribute "
           "The Actors class must instead inherit from Theater::Ticking"
        << std::endl;
    return false;

  case VISIBLE:
    std::cout
        << " Can't assign Internal Attribute 'VISIBLE' via AddActorAttribute "
           "The Actors class must instead inherit from Theater::Visible"
        << std::endl;
    return false;

  case TRANSFORMABLE:
    std::cout
        << " Can't assign Internal Attribute 'TRANSFORMABLE' via "
           "AddActorAttribute "
           "The Actors class must instead inherit from Theater::Transform2D"
        << std::endl;

    return false;

#define STAGE_ATTRIBUTE(name)                                                  \
  case name:                                                                   \
    _handle_##name.insert(act);                                                \
    act->_attributes.insert(attr);                                             \
    return true;
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")
#undef STAGE_ATTRIBUTE

  default:
    return false;
  }
}

inline bool Stage::RemoveActorAttribute(Actor *act, Attributes attr) {

  if (_actorsToClear.find(act) == _actorsToClear.end()) {
    return false;
  }

  switch (attr) {
  case DEAD:
    std::cout << " Can't remove ActorAttribute 'DEAD' via "
                 "Stage::RemoveActorAttribute"
              << std::endl;
    return false;

  case TICKING:
    std::cout << " Can't remove Internal Attribute 'TICKING' via "
                 "RemoveActorAttribute "
              << std::endl;
    return false;

  case VISIBLE:
    std::cout << " Can't remove Internal Attribute 'VISIBLE' via "
                 "RemoveActorAttribute "
              << std::endl;
    return false;

  case TRANSFORMABLE:
    std::cout << " Can't remove Internal Attribute 'TRANSFORMABLE' via "
                 "RemoveActorAttribute "
              << std::endl;
    return false;

#define STAGE_ATTRIBUTE(name)                                                  \
  case name:                                                                   \
    _handle_##name.erase(act);                                                 \
    act->_attributes.erase(attr);                                              \
    return true;
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")
#undef STAGE_ATTRIBUTE

  default:
    return false;
  }
}

inline void Stage::ClearStage() {

  for (Actor *a : _actorsToClear)
    a->OnStageLeave(_play);

  _actorsToClear.clear();
  _handle_TIMER.clear();

#define STAGE_ATTRIBUTE(name) _handle_##name.clear();
  STAGE_ATTRIBUTE(DEAD)
  STAGE_ATTRIBUTE(TICKING)
  STAGE_ATTRIBUTE(TRANSFORMABLE)
  STAGE_ATTRIBUTE(VISIBLE)

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")
#undef STAGE_ATTRIBUTE

  for (int a = 0; a < ACTORLIMIT; a++) {
    _renderNodes[a].obj = NULL;
    _renderNodes[a].next = NULL;
    _renderNodes[a].prev = NULL;
    _renderNodes[a].alive = false;
    _renderNodes[a].index = 0;
  }

  _renderNodeCnt = 0;
}

template <typename T> inline void Stage::ClearActorFromStage(T *a) {

  if (std::is_base_of<Actor, T>::value) {
    ((Actor *)a)->OnStageLeave(_play);
    _actorsToClear.erase(a);
  }

  if (std::is_base_of<Ticking, T>::value) {
    auto fndTick = _handle_TICKING.find(a);
    if (fndTick != _handle_TICKING.end()) {
      _handle_TICKING.erase(fndTick);
    }
  }

  if (std::is_base_of<Transform2D, T>::value) {
    auto fndTrns2D = _handle_TRANSFORMABLE.find((Transform2D *)a);
    if (fndTrns2D != _handle_TRANSFORMABLE.end()) {
      _handle_TRANSFORMABLE.erase(fndTrns2D);
    }
  }

  if (std::is_base_of<Visible, T>::value) {
    auto actorVis = a->_attributes.find(VISIBLE);
    if (actorVis != a->_attributes.end()) {
      MakeActorInvisible((Visible *)a);
    }
  }

#define STAGE_ATTRIBUTE(name)                                                  \
  auto act_##name = _handle_##name.find(a);                                    \
  if (act_##name != _handle_##name.end()) {                                    \
    _handle_##name.erase(act_##name);                                          \
  }

  STAGE_ATTRIBUTE(DEAD)

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")
  STAGE_ATTRIBUTE(VISIBLE);
#undef STAGE_ATTRIBUTE
}

inline std::unordered_set<Actor *>
Stage::GetActorsWithAttribute(Attributes attr) {
  switch (attr) {
  case TICKING:
  case TRANSFORMABLE:
  case VISIBLE:
  case DEAD:
    std::cerr << "can't request Actors for Internal Attributes" << std::endl;
  default:
    return std::unordered_set<Actor *>();

#define STAGE_ATTRIBUTE(name)                                                  \
  case name:                                                                   \
    return _handle_##name;

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif
#undef STAGE_ATTRIBUTE
  }
}

inline void Stage::SetTimer(Timer *t) {
  t->m_TimerValue = 0;
  t->m_NextProgress = 0;
  _handle_TIMER.insert(t);
}
inline void Stage::RemoveTimer(Timer *t) { _handle_TIMER.erase(t); }

} // namespace Theater
#endif
