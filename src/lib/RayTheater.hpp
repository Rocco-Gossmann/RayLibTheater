#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sys/wait.h>
#include <type_traits>
#include <unordered_set>

#include <raylib.h>
#include <vector>

// Something to keep .clangd files with the -DSTAGE_ATTRIBUTE from interfering
#ifdef STAGE_ATTRIBUTE
#undef STAGE_ATTRIBUTE
#endif

// Defining how many actors can be handled each frame
#ifndef ACTORLIMIT
#define ACTORLIMIT 64
#endif

#ifndef RAYTHEATER_H
#define RAYTHEATER_H

namespace Theater {

class Stage; // <== "needed by some classes before Stage is defined
class ActorComponent;

//=============================================================================
// BM: Stage::Attributes
//=============================================================================
enum Attributes {
#ifdef STAGE_ATTRIBUTE
#undef STAGE_ATTRIBUTE
#endif

#define STAGE_ATTRIBUTE(name) name,

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

  STAGE_ATTRIBUTE(TICKING) STAGE_ATTRIBUTE(DEAD) STAGE_ATTRIBUTE(TRANSFORMABLE)
      STAGE_ATTRIBUTE(VISIBLE)

          __STAGE_ATTRIBUTE_COUNT
#undef STAGE_ATTRIBUTE
};

//==============================================================================
// BM: RenderList
//==============================================================================
template <typename T> struct RenderNode {
  int index;
  T *obj;
  bool alive;
  RenderNode<T> *next;
  RenderNode<T> *prev;
};

//=============================================================================
// BM: Play - Struct
//=============================================================================
/** @brief Context provided to each Actor on the Scene */
typedef struct Play {
  /** @brief can be multiplied with an 'per second' value (like "pixels per
   * second") to achive framerate independed timing*/
  float deltaTime = 0.0f;

  /** @brief A reference to the stage, that alows interacting with it */
  Stage *stage = NULL;

  /** @brief a more detailed mousepoisition, that includes sub-pixel space
   * (For resized stages / windows) */
  Vector2 mouseLoc = {};

  /** @brief x-coordinate of the pixel, that the mouse is currently pointing at
   * at the stage */
  int mouseX = 0;

  /** @brief y-coordinate of the pixel, that the mouse is currently pointing at
   * at the stage */
  int mouseY = 0;

  /** @brief The available Stage Width in PX */
  int stageWidth = 0;

  /** @brief The available Stage Height in PX */
  int stageHeight = 0;

  /** @brief Bitlist of mousebuttons pressed this turn */
  unsigned char mouseDown;

  /** @brief Bitlist of mousebuttons held this turn */
  unsigned char mouseHeld;

  /** @brief Bitlist of mousebuttons released this turn */
  unsigned char mouseUp;

} Play;

//=============================================================================
// BM: Actor - Class
//=============================================================================
class Actor {
  friend class Stage;
  friend ActorComponent;

public:
  Actor() : _attributes() {}
  bool hasAttribute(Attributes attr) {
    return _attributes.find(attr) != _attributes.end();
  }

private:
  virtual void OnStageEnter(Play) {}
  virtual void OnStageLeave(Play) {}

private:
  std::unordered_set<Attributes> _attributes;
};

//==============================================================================
// BM: ActorComponent - Class
//==============================================================================
class ActorComponent {
  friend Stage;

public:
  ActorComponent(Actor *ac, Attributes at) { ac->_attributes.insert(at); }
};

//==============================================================================
// BM: ActorComponent - Transform2D - Class
//==============================================================================
class Transform2D : ActorComponent {
  friend Stage;

public:
  Transform2D(Actor *a)
      : ActorComponent(a, TRANSFORMABLE), _loc({0.0f, 0.0f}),
        loc({0.0f, 0.0f}){};
  Vector2 getLoc() { return Vector2(loc); }

  void setLoc(Vector2 l) {
    this->_loc.x = l.x;
    this->_loc.y = l.y;
  }

private:
  // Values, that the Actor is currently at
  Vector2 loc;

  // Values, that the Actor should be next frame
  Vector2 _loc;

  void FlipTransform2DStates() {
    loc.x = _loc.x;
    loc.y = _loc.y;
  }
};

//==============================================================================
// BM: ActorComponent - Ticking - Class
//==============================================================================
class Ticking : ActorComponent {
  friend Stage;

public:
  Ticking(Actor *ac) : ActorComponent(ac, TICKING) {}

private:
  virtual bool OnTick(Play) = 0;
};

//==============================================================================
// BM: ActorComponent - Visible - Class
//==============================================================================
class Visible : ActorComponent {
  friend Stage;

public:
  Visible(Actor *ac) : ActorComponent(ac, VISIBLE) {}
  /**
   * @brief Sets what elements are drawn above which other
   * actors with higher numbers will be drawn above elements with lower numbers
   * Should 2 elements have the same number, there draw order
   * depends on the order, in which they were made visible
   *
   * @param layer
   */
  void SetRenderLayer(int layer) { this->_zindex = layer; }

private:
  int _renderListIndex = -1;
  int _zindex = 0;
  virtual void OnDraw(Play) = 0;
};

//=============================================================================
// BM: Scene - Class
//=============================================================================
class Scene {
  friend Stage;

public:
  virtual void OnStart(Play) {}
  virtual void OnUpdate(Play) {}
  virtual void OnStageDraw(Play) {}
  virtual void OnWindowDraw(Play) {}
  virtual void OnEnd(Play) {}

  void TransitionTo(Scene *s) {
    _followup = s;
    _transitionRequested = true;
  }

  Scene *followup() { return _followup; }

private:
  bool _transitionRequested = false;
  Scene *_followup = NULL;

  bool Tick(Play p) {
    OnUpdate(p);
    return !_transitionRequested;
  }

  Scene *Unload(Play p) {
    OnEnd(p);
    _transitionRequested = false;
    return _followup;
  }
};

//=============================================================================
// BM: Stage - Class
//=============================================================================
class Stage {
  friend class Builder;

public:
  ~Stage() { _scene = NULL; }

  /**
   * @brief Changes the Color of the Border, that is show, when the window is
   * scaled to an aspect ratio different, than the Stages
   */
  void BorderColor(Color);

  /**
   * @brief Background-Color of the stage. (Each frame, the stage is cleared
   * to this color.
   */
  void BackgroundColor(Color);

  /**
   * @brief Registers an Actor as being on the stage
   *
   * @tparam T any class that implement Theater::Actor
   * @param a - reference that the Stage will use to adress the Actor
   */
  template <typename T> void AddActor(T *a);

  /**
   * @brief Marks any Actor as "DEAD", meaning, it will be removed from the
   * stage between cycles
   *
   * @param a the pointer to the actor to remove (Must be the same value as
   * given in AddActor)
   */
  template <typename T> void RemoveActor(T *a);

  /**
   * @brief By default Actors are invisible / Not rendered
   * use this function to make it visible (Add it to the Stages render-list)
   *
   * @tparam T any class that implements Theater::Actor and Theater::Visible
   * @return true on success ; false == to many Actors are already on stage or
   * you called this function during rendering
   */
  template <typename T> bool MakeActorVisible(T *);

  /**
   * @brief removes any visible Actor from the stages render-list
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

private:
  Stage(int width, int height, float scale = 1.0);

  RenderNode<Visible> _renderNodes[ACTORLIMIT];
  unsigned int _renderNodeCnt;
  RenderNode<Visible> _renderNodeRoot;

  const char *_stageTitle;
  float _stageWidth;
  float _stageHeight;
  float _stageScale;
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

  std::unordered_set<Actor *> _actorsToClear;
  std::unordered_set<Ticking *> _handle_TICKING;
  std::unordered_set<Transform2D *> _handle_TRANSFORMABLE;
  std::unordered_set<Actor *> _handle_DEAD;

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

//=============================================================================
// BM: Builder - Class
//=============================================================================
/**
 * @class Builder
 * @brief meant to provide an easy way to setup a Stage.
 */
class Builder {
public:
  /**
   * @brief
   *
   * @param width  - the stages with in pixels
   * @param height - the stages height in pixels
   * @param scale  - multiplier by which the window will be scaled (e.g. 2.0 =
   * each pixel of the stage, takes 2 pixels in the Window)
   */
  Builder(int width, int height, float scale = 1.0)
      : _stage(Stage(width, height, scale)) {}

  /**
   * @brief Sets the Title for the opened Window
   *
   * @param t - title
   * @return  itself, for easy chaining of setters
   */
  Builder Title(const char *t) {
    _stage._stageTitle = t;
    return *this;
  }

  /**
   * @brief changes the color fo the border, that is shown, when
   * the window is scaled to a different aspect ratio
   *
   * @param c  - Raylib Color
   * @return  itself for easy chaining of setters
   */
  Builder BorderColor(Color c) {
    _stage._borderColor = c;
    return *this;
  }

  /**
   * @brief sets the cleard background-color of the Stage
   *
   * @param c - Raylib Color
   * @return  itself for easy chainging of setters
   */
  Builder BackgroundColor(Color c) {
    _stage._backgroundColor = c;
    return *this;
  }

  /**
   * @brief Opens the window and starts playing the given Scene
   * @param sc
   */
  void Play(Scene *sc) { _stage.Play(sc); }

private:
  Stage _stage;
};

//==============================================================================
// BM: Stage - Implementation
//------------------------------------------------------------------------------
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
      _rendering(false) {

  static_assert(ACTORLIMIT > 0, "Set ACTORLIMIT must be bigger than 0");

  // Attributes Initialized
  //=========================================================================
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

//==============================================================================
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
    _play.mouseHeld = 0;
    _play.mouseUp = 0;
    _play.mouseDown = 0;

    for (unsigned char a = 0; a < 6; a++) {
      _play.mouseDown |= (IsMouseButtonPressed(a) ? 1 : 0) << a;
      _play.mouseHeld |= (IsMouseButtonDown(a) ? 1 : 0) << a;
      _play.mouseUp |= (IsMouseButtonUp(a) ? 1 : 0) << a;
    }

    // Just in case held and Pressed overlap => remove Pressed from held.
    _play.mouseHeld &= ~_play.mouseDown;

    // Tick all the actors
    for (Ticking *ticker : _handle_TICKING)
      ticker->OnTick(_play);

    // Figure out the Render order of actors;
    int rnCnt = _renderNodeCnt;
    RenderNode<Visible> *rn = &_renderNodeRoot;
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

  _stageScale = scale;
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

/**
 * @brief Will remove an Actor from the stage, on the start of the next Cycle
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

/**
 * @brief Activates the Rendinger of the actor on the stage
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

/**
 * @brief Makes sure an actor is no longer visible on stage, but keeps
 * it on the stage.
 *
 * Can only execute, when not rendering.
 *
 * @tparam T  - Any Class extending Theater::Actor and Theater::Visible
 * @param actor - an instance of the A Class extending Theater::Actor and
 */
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

#define STAGE_ATTRIBUTE(name) _handle_##name.clear();
  STAGE_ATTRIBUTE(DEAD)
  STAGE_ATTRIBUTE(TICKING)
  STAGE_ATTRIBUTE(TRANSFORMABLE)

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")
  STAGE_ATTRIBUTE(VISIBLE)
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
    auto fndTick = _handle_TICKING.find((Ticking *)a);
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
      int idx = ((Visible *)a)->_renderListIndex;
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
  STAGE_ATTRIBUTE(VISIBLE)
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

//==============================================================================
// BM: Collider - Class
//==============================================================================
class ColliderPoint;
class ColliderCircle;
class ColliderRect;

class Collider {
  virtual bool isCollidingWithPoint(ColliderPoint *) = 0;
  virtual bool isCollidingWithRect(ColliderRect *) = 0;
  virtual bool isCollidingWithCircle(ColliderCircle *) = 0;

  virtual bool containsPoint(float x, float y) = 0;
};

//==============================================================================
// BM: Collider - Point - Class
//==============================================================================
class ColliderPoint : public Collider {
public:
  bool isCollidingWithPoint(ColliderPoint *) override;
  bool isCollidingWithRect(ColliderRect *) override;
  bool isCollidingWithCircle(ColliderCircle *) override;

  bool containsPoint(float x, float y) override;

  virtual Vector2 getPosition() = 0;

}; // namespace Theater

//==============================================================================
// BM: Collider - Rect - Class
//==============================================================================
class ColliderRect : public Collider {
  // Virtual methods
public:
  virtual Rectangle getRect() = 0;

public:
  bool isCollidingWithPoint(ColliderPoint *) override;
  bool isCollidingWithRect(ColliderRect *) override;
  bool isCollidingWithCircle(ColliderCircle *) override;

  bool rectContainsPoint(Rectangle r, Vector2 p);
  bool containsPoint(float x, float y) override;

private:
  bool rectInRect(Rectangle r1, Rectangle r2);

}; // namespace Theater

//==============================================================================
// BM: Collider - Circle - Class
//==============================================================================
class ColliderCircle : public Collider {
public:
  bool isCollidingWithPoint(ColliderPoint *) override;
  bool isCollidingWithRect(ColliderRect *) override;
  bool isCollidingWithCircle(ColliderCircle *) override;

  virtual Vector2 getPosition() = 0;
  virtual float getRadius() = 0;

  bool containsPoint(float x, float y) override;
  bool containsPoint(Vector2 p);

private:
  bool lineHitsCircle(float cx, float cy, float radius, float x1, float y1,
                      float x2, float y2);

  bool pointHitsCircle(float cx, float cy, float radius, float px, float py);

  bool circleHitsPolyShape(Vector2 circleOrigin, float circleRadius,
                           std::vector<Vector2> lst);
};

//=======================================0=======================================
// BM: Collider - Point - Implementation
//==============================================================================
inline bool ColliderPoint::containsPoint(float x, float y) {
  auto p1 = this->getPosition();
  return (p1.x == x && p1.y == y);
};

inline bool ColliderPoint::isCollidingWithPoint(ColliderPoint *p) {
  auto p2 = p->getPosition();
  return containsPoint(p2.x, p2.y);
}

inline bool ColliderPoint::isCollidingWithRect(ColliderRect *r) {
  return r->rectContainsPoint(r->getRect(), this->getPosition());
}

inline bool ColliderPoint::isCollidingWithCircle(ColliderCircle *c) {
  return c->containsPoint(getPosition());
}

//==============================================================================
// BM: Collider - Rect - Implementation
//==============================================================================
inline bool ColliderRect::rectContainsPoint(Rectangle r, Vector2 p) {
  return p.x >= r.x && p.x <= r.x + r.width && p.y >= r.y &&
         p.y <= r.y + r.height;
}

inline bool ColliderRect::containsPoint(float x, float y) {
  auto r = this->getRect();
  return x >= r.x && x <= r.x + r.width && y >= r.y && y <= r.y + r.height;
};

inline bool ColliderRect::rectInRect(Rectangle r1, Rectangle r2) {

  bool tlIn = rectContainsPoint(r1, {r2.x, r2.y});
  bool trIn = rectContainsPoint(r1, {r2.x + r2.width, r2.y});
  bool blIn = rectContainsPoint(r1, {r2.x, r2.y + r2.height});
  bool brIn = rectContainsPoint(r1, {r2.x + r2.width, r2.y + r2.height});

  return (tlIn || trIn || blIn || brIn);
}

inline bool ColliderRect::isCollidingWithPoint(ColliderPoint *p) {
  return this->rectContainsPoint(this->getRect(), p->getPosition());
}

inline bool ColliderRect::isCollidingWithCircle(ColliderCircle *c) {
  return c->isCollidingWithRect(this);
}

inline bool ColliderRect::isCollidingWithRect(ColliderRect *r) {
  auto r1 = this->getRect();
  auto r2 = r->getRect();
  return rectInRect(r1, r2) || rectInRect(r2, r2);
}

//==============================================================================
// BM: Collider - Circle - Implementation
//==============================================================================
inline bool ColliderCircle::pointHitsCircle(float cx, float cy, float rad,
                                            float px, float py) {
  auto rad2 = rad * rad;
  auto dstx1 = abs(cx - px);
  auto dsty1 = abs(cy - py);

  return dstx1 * dstx1 + dsty1 * dsty1 > rad2;
}

inline bool ColliderCircle::containsPoint(float x, float y) {
  auto p = this->getPosition();
  auto r = this->getRadius();
  return pointHitsCircle(p.x, p.y, r, x, y);
};

inline bool ColliderCircle::lineHitsCircle(float cx, float cy, float cr,
                                           float x1, float y1, float x2,
                                           float y2) {

  float lineX = x2 - x1; // Get distances of Line Vector
  float lineY = y2 - y1;
  float lineL = lineX * lineX + lineY * lineY;

  float circleX = cx - x1; // Get Distances for Circle to Line Vector
  float circleY = cy - y1;

  // Find Closest normalized point on Line to get from line to Circle Origin
  float t = fmax(0, fmin(lineL, (lineX * circleX + lineY * circleY))) / lineL;

  // Translate that normalized point back to the point on the Line
  float offsetX = x1 + ((x2 - x1) * t);
  float offsetY = y1 + ((y2 - y1) * t);

  // do a simple circle to point collision check with the found point on the
  // Line
  return (((cx - offsetX) * (cx - offsetX) + (cy - offsetY) * (cy - offsetY)) <=
          (cr * cr));
}

inline bool ColliderCircle::circleHitsPolyShape(Vector2 circleOrigin,
                                                float circleRadius,
                                                std::vector<Vector2> lst) {
  bool hit = false;
  for (auto a = 0; a < lst.size() - 1; a++) {
    auto s = lst.at(a);
    auto e = lst.at(a + 1);
    hit = hit || this->lineHitsCircle(circleOrigin.x, circleOrigin.y,
                                      circleRadius, s.x, s.y, e.x, e.y);
  }

  return hit;
}

inline bool ColliderCircle::containsPoint(Vector2 p) {
  auto rad = getRadius();
  auto rad2 = rad * rad;

  auto pc = getPosition();

  auto dstx1 = abs(pc.x - p.x);
  auto dsty1 = abs(pc.y - p.y);

  return dstx1 * dstx1 + dsty1 * dsty1 > rad2;
}

inline bool ColliderCircle::isCollidingWithPoint(ColliderPoint *p) {
  return containsPoint(p->getPosition());
}

inline bool ColliderCircle::isCollidingWithCircle(ColliderCircle *c) {
  auto rad = this->getRadius() + c->getRadius();
  auto rad2 = rad * rad;

  auto p = getPosition();
  auto pc = c->getPosition();

  auto dstx1 = abs(pc.x - p.x);
  auto dsty1 = abs(pc.y - p.y);

  return dstx1 * dstx1 + dsty1 * dsty1 > rad2;
}

inline bool ColliderCircle::isCollidingWithRect(ColliderRect *rc) {
  auto o = this->getPosition();
  auto r = rc->getRect();
  auto rad = this->getRadius();

  return rc->rectContainsPoint(r, o) ||
         circleHitsPolyShape(o, rad,
                             {{r.x, r.y},
                              {r.x + r.width, r.y},
                              {r.x + r.width, r.y + r.height},
                              {r.x, r.y + r.height},
                              {r.x, r.y}});
}

}; // namespace Theater
#endif
