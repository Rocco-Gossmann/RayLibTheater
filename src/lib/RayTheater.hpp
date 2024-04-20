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

#ifndef RAYTHEATER_H
#define RAYTHEATER_H

namespace Theater {

class Stage; // <== "needed by some clases before Stage is defined
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

      __STAGE_ATTRIBUTE_COUNT
#undef STAGE_ATTRIBUTE
};

//=============================================================================
// Stage::Play
//=============================================================================
/** @brief Context provided to each Actor on the Scene */
typedef struct Play {
  float deltaTime = 0.0f;
  Stage *stage = NULL;
  Vector2 mouseLoc = {};
  int mouseX = 0;
  int mouseY = 0;
} Play;

//=============================================================================
// Stage::Actor
//=============================================================================
class Actor {
  friend class Stage;
  friend ActorComponent;

public:
  Actor() : _attributes() {}

private:
  std::unordered_set<Attributes> _attributes;
};

//==============================================================================
// Stage::ActorCompnent
//==============================================================================
class ActorComponent {
public:
  ActorComponent(Actor *ac, Attributes at) { ac->_attributes.insert(at); }
};

//==============================================================================
// Stage::ActorCompnent Transform2D
//==============================================================================
class Transform2D : ActorComponent {

public:
  Transform2D(Actor *a)
      : ActorComponent(a, TRANSFORMABLE), _loc({0.0f, 0.0f}),
        loc({0.0f, 0.0f}){};
  Vector2 getLoc() { return Vector2(loc); }

  void setLoc(Vector2 l) {
    this->_loc.x = l.x;
    this->_loc.y = l.y;
  }

  void FlipTransform2DStates() {
    loc.x = _loc.x;
    loc.y = _loc.y;
  }

private:
  // Values, that the Actor is currently at
  Vector2 loc;

  // Values, that the Actor should be next frame
  Vector2 _loc;
};

//==============================================================================
// Stage::ActorCompnent Ticking
//==============================================================================
class Ticking : ActorComponent {
public:
  Ticking(Actor *ac) : ActorComponent(ac, TICKING) {}
  virtual bool OnTick(Play) = 0;
  int blub = 0;
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

  template <typename T> void AddActor(T *a) {
    static_assert(std::is_base_of<Actor, T>::value,
                  "Can't add class, that does not inherit from Theater::Actor");

    if (std::is_base_of<Ticking, T>::value)
      _handle_TICKING.insert((Ticking *)a);

    if (std::is_base_of<Transform2D, T>::value)
      _handle_TRANSFORMABLE.insert((Transform2D *)a);
  }
  void RemoveActor(Actor *a);

private:
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

  std::unordered_set<Ticking *> _handle_TICKING;
  std::unordered_set<Transform2D *> _handle_TRANSFORMABLE;
  std::unordered_set<Actor *> _handle_DEAD;

#define STAGE_ATTRIBUTE(name) std::unordered_set<Actor *> _handle_##name;
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

#undef STAGE_ATTRIBUTE

  void Play(Scene *sc);

  void switchScene(Scene *);
  void onResize();

  Stage(int width, int height, float scale = 1.0)
      : _scene(0x0), _viewportOrigin({0, 0}),
        _stageRect(
            {0, 0, static_cast<float>(width), static_cast<float>(-height)}),
        _viewportRect({0, 0, static_cast<float>(width) * scale,
                       static_cast<float>(height) * scale}),
        _stageWidth(width), _stageHeight(height), _play(),
        _backgroundColor(Color{0x00, 0x00, 0xAA, 0xff}),
        _borderColor(Color{0x00, 0x88, 0xff, 0xff}), _handle_TICKING(),
        _handle_DEAD(), _handle_TRANSFORMABLE(), _stageScale(scale),
        _stageTitle("< RayWrapC - Project >") {

#define STAGE_ATTRIBUTE(name) _handle_##name = std::unordered_set<Actor *>();
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

#undef STAGE_ATTRIBUTE
  }

  void ClearActorFromStage(Actor *a);
  void ClearStage();
};

//=============================================================================
// Stage::Builder
//=============================================================================
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

    // TODO: calculate mouse position on stage
    _play.mouseLoc.x -= _viewportRect.x;
    _play.mouseLoc.y -= _viewportRect.y;

    _play.mouseLoc.x /= _stageScale;
    _play.mouseLoc.y /= _stageScale;

    _play.mouseX = std::floor(_play.mouseLoc.x);
    _play.mouseY = std::floor(_play.mouseLoc.y);

    // Tick all the actors
    for (Ticking *ticker : _handle_TICKING)
      ticker->OnTick(_play);

    // Start drawing on the Stage
    BeginTextureMode(_stage);
    ClearBackground(_backgroundColor);
    _scene->OnStageDraw(_play);
    EndTextureMode();

    // Start drawing on the Stage
    BeginDrawing();
    ClearBackground(_borderColor);
    DrawTexturePro(_stage.texture, _stageRect, _viewportRect, _viewportOrigin,
                   0, WHITE);
    _scene->OnWindowDraw(_play);
    EndDrawing();
  }

  switchScene(0);
  UnloadRenderTexture(_stage);
  ClearStage();
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

inline void Stage::RemoveActor(Actor *a) {
  a->_attributes.insert(DEAD);
  _handle_DEAD.insert(a);
}

inline void Stage::ClearStage() {

#define STAGE_ATTRIBUTE(name) _handle_##name.clear();
  STAGE_ATTRIBUTE(DEAD)
  STAGE_ATTRIBUTE(TICKING)
  STAGE_ATTRIBUTE(TRANSFORMABLE)

#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

#undef STAGE_ATTRIBUTE
}

inline void Stage::ClearActorFromStage(Actor *a) {

  auto fndTick = _handle_TICKING.find((Ticking *)a);
  if (fndTick != _handle_TICKING.end()) {
    _handle_TICKING.erase(fndTick);
  }

  auto fndTrns2D = _handle_TRANSFORMABLE.find((Transform2D *)a);
  if (fndTrns2D != _handle_TRANSFORMABLE.end()) {
    _handle_TRANSFORMABLE.erase(fndTrns2D);
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

#undef STAGE_ATTRIBUTE
}

} // namespace Theater

#endif
