#include <climits>
#include <cmath>
#include <cstdio>
#include <sys/wait.h>
#include <type_traits>
#include <unordered_set>

#include <raylib.h>

// Something to keep .clangd files with the -DSTAGE_ATTRIBUTE from interfering
#ifdef STAGE_ATTRIBUTE
#undef STAGE_ATTRIBUTE
#endif

// Defining how many actors can be handled each frame
#ifndef ACTORLIMIT
#define ACTORLIMIT 256
#endif

#ifndef RAYTHEATER_H
#define RAYTHEATER_H

namespace Theater {

class Stage; // <== "needed by some classes before Stage is defined
class ActorComponent;

//=============================================================================
// Stage::Attributes
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
// RenderList
//==============================================================================
template <typename T> struct RenderNode {
  int index;
  T *obj;
  bool alive;
  RenderNode<T> *next;
  RenderNode<T> *prev;
};

//=============================================================================
// Stage::Play
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

} Play;

//=============================================================================
// Stage::Actor
//=============================================================================
class Actor {
  friend class Stage;
  friend ActorComponent;

public:
  Actor() : _attributes() {}
  virtual void OnStageEnter(Play) {}
  virtual void OnStageLeave(Play) {}

private:
  std::unordered_set<Attributes> _attributes;
};

//==============================================================================
// Stage::ActorCompnent
//==============================================================================
class ActorComponent {
  friend Stage;

public:
  ActorComponent(Actor *ac, Attributes at) { ac->_attributes.insert(at); }
};

//==============================================================================
// Stage::ActorCompnent Transform2D
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
// Stage::ActorCompnent Ticking
//==============================================================================
class Ticking : ActorComponent {
  friend Stage;

public:
  Ticking(Actor *ac) : ActorComponent(ac, TICKING) {}

private:
  virtual bool OnTick(Play) = 0;
};

//==============================================================================
// Stage::ActorCompnent Visible
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
  bool _renderListIndex = -1;
  bool _zindex = 0;
  virtual void OnDraw(Play) = 0;
};

//=============================================================================
// Stage::Scene
//=============================================================================
class Scene {
public:
  virtual void OnLoad(Play) {}
  virtual Scene *OnUnload(Play) { return NULL; }
  virtual void OnStageDraw(Play) {}
  virtual void OnWindowDraw(Play) {}

  virtual bool OnTick(Play) { return false; }
};

//=============================================================================
// Stage::Stage
//=============================================================================
class Stage {
  friend class Builder;

public:
  ~Stage() { _scene = NULL; }

  Stage BorderColor(Color);
  Stage BackgroundColor(Color);

  template <typename T> void AddActor(T *a);
  void RemoveActor(Actor *a);

  template <typename T> bool MakeActorVisible(T *);
  template <typename T> void MakeActorInvisible(T *);

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

  std::unordered_set<Actor *> _actorsToClear;
  std::unordered_set<Ticking *> _handle_TICKING;
  std::unordered_set<Transform2D *> _handle_TRANSFORMABLE;
  std::unordered_set<Actor *> _handle_DEAD;

#define STAGE_ATTRIBUTE(name) std::unordered_set<Actor *> _handle_##name;
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")
  STAGE_ATTRIBUTE(VISIBLE);
#undef STAGE_ATTRIBUTE

  void Play(Scene *sc);

  void switchScene(Scene *);
  void onResize();

  template <typename T> void ClearActorFromStage(T *a);
  void ClearStage();
};

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
// Stage Implementation
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
    if (!_scene->OnTick(_play)) {
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

    // Tick all the actors
    for (Ticking *ticker : _handle_TICKING)
      ticker->OnTick(_play);

    // Figure out the Render order of actors;
    int rnCnt = _renderNodeCnt;
    RenderNode<Visible> *rn = &_renderNodeRoot;
    rn->next = NULL;
    for (int a = 0; a < rnCnt; a++) {
      auto node = &(_renderNodes[a]);

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

      } else if (node->index < rn->index && rn->prev == NULL) {
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
  if (_scene != NULL) {

    if (sc == NULL)
      sc = _scene->OnUnload(_play);
    else
      _scene->OnUnload(_play);

    _scene = NULL;
  }

  if (sc != NULL) {
    sc->OnLoad(_play);
    _scene = sc;
  }
}

inline Stage Stage::BorderColor(Color c) {
  _borderColor = c;
  return *this;
}

inline Stage Stage::BackgroundColor(Color c) {
  _backgroundColor = c;
  return *this;
}

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

  ((Actor *)a)->OnStageEnter(_play);
  _actorsToClear.insert(a);

  if (std::is_base_of<Ticking, T>::value)
    _handle_TICKING.insert((Ticking *)a);

  if (std::is_base_of<Transform2D, T>::value)
    _handle_TRANSFORMABLE.insert((Transform2D *)a);
}

/**
 * @brief Will remove an Actor from the stage, on the start of the next Cycle
 *
 * @param a - the actor to remove
 */
inline void Stage::RemoveActor(Actor *a) {
  a->_attributes.insert(DEAD);
  _handle_DEAD.insert(a);
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

  // If the removed element is not the last one.
  if (vis->_renderListIndex != _renderNodeCnt - 1) {
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
  _renderNodeCnt--;
  _renderNodes[_renderNodeCnt].obj = NULL;
  _renderNodes[_renderNodeCnt].alive = false;
  _renderNodes[_renderNodeCnt].next = NULL;
  _renderNodes[_renderNodeCnt].prev = NULL;
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

} // namespace Theater

#endif
