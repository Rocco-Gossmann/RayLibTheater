#include <functional>
#ifndef RAYTHEATER_H
#define RAYTHEATER_H 1

#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
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

namespace Theater {

class Stage; // <== "needed by some classes before Stage is defined
class ActorComponent;

#ifdef STAGE_ATTRIBUTE
#undef STAGE_ATTRIBUTE
#endif

// BM: Attributes - Enum
//=============================================================================
enum Attributes {

#define STAGE_ATTRIBUTE(name) name,

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

  STAGE_ATTRIBUTE(TICKING) STAGE_ATTRIBUTE(DEAD) STAGE_ATTRIBUTE(TRANSFORMABLE)
      STAGE_ATTRIBUTE(VISIBLE)

          __STAGE_ATTRIBUTE_COUNT
#undef STAGE_ATTRIBUTE
};

// BM: RenderNode - Struct
//==============================================================================
template <typename T> struct RenderNode {
  int index;
  T *obj;
  bool alive;
  RenderNode<T> *next;
  RenderNode<T> *prev;
};

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

  /** @brief Bitlist of mousebuttons just pressed this turn
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseDown;

  /** @brief Bitlist of mousebuttons held this turn
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseHeld;

  /** @brief Bitlist of mousebuttons not currently held or pressed
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseUp;

  /** @brief Bitlist of mousebuttons that where released last Frame
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseReleased;

} Play;

// BM: Actor - Class
//==============================================================================
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

// BM: ActorComponent - Class
//==============================================================================
class ActorComponent {
  friend Stage;

public:
  ActorComponent(Actor *ac, Attributes at) { ac->_attributes.insert(at); }
};

// BM: ActorComponent - Transform2D - Class
//==============================================================================
class Transform2D : ActorComponent {
  friend Stage;

public:
  Transform2D(Actor *a)
      : ActorComponent(a, TRANSFORMABLE), _loc({0.0f, 0.0f}),
        loc({0.0f, 0.0f}){};

  /** @brief privides the Actors location, at the star of the cycle
   * @return the actors location
   */
  Vector2 getLoc() { return Vector2(loc); }

  /** @brief Requests the Actor to change its location at the beginning
   * of the next Cycle
   * @param l the new Location, the Actor should move to
   */
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

// BM: ActorComponent - Ticking - Class
//==============================================================================
class Ticking : ActorComponent {
  friend Stage;

public:
  Ticking(Actor *ac) : ActorComponent(ac, TICKING) {}

private:
  virtual void OnTick(Play) = 0;
};

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

// BM: Timer - Class
//==============================================================================
class Timer {
public:
  Timer() noexcept;

  typedef std::function<void(Timer *, float passTime, float goalTime)>
      t_TimerProgressHandler;

  typedef std::function<void(Timer *)> t_TimerFinishHandler;

  /** @brief restarts the timer for the given number of milliseconds */
  void setStart(float milliseconds);

  /** @brief changes the timers target time, but keeps the timer running (think
   * of it as advancing a kitchen timer, without letting it run out first) */
  void updateGoal(float milliseconds);

  /** @brief set a handler for processing timer progress
   * @param progressCooldown - min delay between handler calls
   * @param handler - lambda function called on each progress step
   */
  void onProgress(float progressCooldown, t_TimerProgressHandler handler);

  /** @brief sets the handler to be called, once the timer finished
   * @param handler
   */
  void onFinish(t_TimerFinishHandler handler);

private:
  /** @brief keeps track of that the current timers goal is*/
  float m_TimerGoal;

  /** @brief Milliseconds passed since the timer started */
  float m_TimerValue;

  /** @brief keeps track of that the current timers goal is*/
  float m_ActiveTimerGoal;

  /** @brief Milliseconds passed since the timer started */
  float m_ActiveTimerValue;

  /** @brief called, once the fimer reaches its target */
  t_TimerFinishHandler m_OnFinish;

  /** @brief called, once the fimer reaches its target */
  t_TimerProgressHandler m_OnProgress;

  /** @brief milliseconds to wait, before sending the next progress event */
  float m_ProgressResolution = 0;

  /** @brief keeps track of at what progress to fire the next Event */
  float m_NextProgress = 0;

private:
  friend class Stage;
  void stageUpdate(Play p);
};

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

// BM: Stage - Class
//=============================================================================
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
  bool _tickingPaused;

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

// BM: Stage - Implementation
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
      _rendering(false), _tickingPaused(false) {

  static_assert(ACTORLIMIT > 0, "Set ACTORLIMIT must be bigger than 0");

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

    // Tick all the actors
    if (!_tickingPaused)
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

// BM: Timer - Implementation
//==============================================================================
inline Timer::Timer() noexcept
    : m_TimerGoal(0), m_TimerValue(0), m_ActiveTimerGoal(0),
      m_ActiveTimerValue(0), m_OnFinish([](Timer *t) {}),
      m_OnProgress([](Timer *o, float t, float g) {}) {}

inline void Timer::setStart(float millisecs) {
  m_TimerValue = 0;
  m_TimerGoal = millisecs;
}

inline void Timer::updateGoal(float millisecs) { m_TimerGoal = millisecs; }

inline void Timer::onProgress(float progressCooldown,
                              t_TimerProgressHandler handler) {
  m_ProgressResolution = progressCooldown;
  m_OnProgress = handler;
}

inline void Timer::onFinish(t_TimerFinishHandler handler) {
  m_OnFinish = handler;
}

// TODO: Continue here

}; // namespace Theater

#endif // RAYTHEATER_H
