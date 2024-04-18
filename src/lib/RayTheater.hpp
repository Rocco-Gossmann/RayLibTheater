#include <cstdio>
#include <sys/wait.h>
#include <type_traits>
#include <unordered_set>

#include <raylib.h>

#ifndef RAYTHEATER_H
#define RAYTHEATER_H

namespace StageHelpers {
//=============================================================================
// Stage::DoubleLinkedList
//=============================================================================
template <typename T> class DoubleLinkedList {
public:
  DoubleLinkedList<T>(bool isHead = true) : _next(0), _prev(0), _content(0) {
    _head = this;
    _tail = this;
    if (isHead) {
      _content = new std::unordered_set<T *>();
    }
  };

  /**
   * @brief Add a new Item to list, after this Node
   *
   * @param item
   * @return - the node in the list, that was created
   */
  DoubleLinkedList<T> *Append(T *item) {
    if (item == 0)
      throw "cant add NULL / 0 / nullptr to a linked list";

    auto node = newNode();
    node->_item = item;

    // Set new tail, when appending to the last element of the list
    if (this->_next == 0)
      this->_head->_tail = node;

    // Connect the new node to the list
    node->_prev = this;
    node->_next = this->_next;
    this->_next = node;
    this->_content->insert(item);

    return node;
  }

  /**
   * @brief Add a new Item to the very end of the List
   *
   * @param item
   * @return - the node in the list, that was created
   */
  DoubleLinkedList<T> *Push(T *item) { return this->_tail->Append(item); }

  /**
   * @brief Add a new Item to list, before this Node
   *
   * If this node is the root of a list, the item is added after this node
   *
   * @param item
   * @return - the node in the list, that was created
   */
  DoubleLinkedList<T> *Prepend(T *item) {
    if (item == 0)
      throw "cant add NULL / 0 / nullptr to a linked list";

    auto node = newNode();
    node->_item = item;

    // Set new tail, when appending to the last element of the list
    if (this->_prev == 0) {
      // Cant prepend the the _head of a list, append to the head instead
      // (chaning the first elment of the list)
      return this->Append(item);
    }

    node->_next = this;
    node->_prev = this->_prev;
    this->_prev = node;
    this->_content->insert(item);

    return node;
  }

  /**
   * @brief finds the node of an item after this node.
   *
   * @param item - 0 if the item could not be found
   */
  DoubleLinkedList<T> *Find(T *item) {
    if (this->_content->find(item) == this->_content->end())
      return 0;

    auto node = this->_head;
    while ((node = node->Next()) != 0) {
      if (node->_item == item)
        return node;
    }

    return 0;
  }

  DoubleLinkedList<T> *Next() { return _next; }

  void Drop() {
    if (_tail != 0)
      throw "can't drop the root of a DoubleLinkedList";

    if (_prev != 0)
      _prev->_next = this->_next;
    if (_next != 0)
      _next->_prev = this->_prev;

    delete (this);
  };

protected:
  T *_item;
  DoubleLinkedList<T> *_next;
  DoubleLinkedList<T> *_prev;
  DoubleLinkedList<T> *_head;
  DoubleLinkedList<T> *_tail;

private:
  DoubleLinkedList<T> *newNode() {
    auto node = new DoubleLinkedList<T>(false);
    node->_head = this->_head;
    node->_tail = 0; // Only the head can know the Tail
    node->_content = this->_head->_content;
    return node;
  }

  std::unordered_set<T *> *_content;
};

} // namespace StageHelpers

namespace Stage {

class Stage; // <== "needed by some clases before Stage is defined
class ActorComponent;

//=============================================================================
// Stage::Attributes
//=============================================================================
enum Attributes {
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
typedef struct {
  float deltaTime;
  Stage *stage;
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
  Transform2D(Actor *a) : ActorComponent(a, TRANSFORMABLE){};
  Vector2 getLoc() { return Vector2(loc); }

  void setLoc(Vector2 l) {
    this->_loc.x = l.x;
    this->_loc.y = l.y;
  }

  void FlipStates() {
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
                  "Can't add class, that does not inherit from Stage::Actor");

    for (auto attr : a->_attributes) {
      switch (attr) {
      case TICKING:
        static_assert(std::is_base_of<Ticking, T>::value,
                      "Warning !!! class that does not inherit from "
                      "Stage::Ticking has TICKABLE Attribute");
        _handle_TICKING.insert(a);
        break;

      case TRANSFORMABLE:
        static_assert(std::is_base_of<Transform2D, T>::value,
                      "Warning !!! class that does not inherit from "
                      "Stage::Transform2D has TRANSFORMABLE Attribute");
        _handle_TRANSFORMABLE.insert(a);
        break;

      default:
        break;
      }
    }
  }
  void RemoveActor(Actor *a);

private:
  const char *_stageTitle;
  float _stageWidth;
  float _stageHeight;
  RenderTexture2D _stage;
  Scene *_scene;
  Rectangle _viewportRect;
  Rectangle _stageRect;
  Vector2 _viewportOrigin;

  Color _borderColor;
  Color _backgroundColor;

  ::Stage::Play _play;

  std::unordered_set<Ticking *> _handle_TICKING;
  std::unordered_set<Transform2D *> _handle_TRANSFORMABLE;
  std::unordered_set<Actor *> _handle_DEAD;

#define STAGE_ATTRIBUTE(name) std::unordered_set<Actor *> _handle_##name;
#if __has_include("RayTheaterAttributes.hpp")
#include "RayTheaterAttributes.hpp"
#endif // __has_include("RayTheaterAttributes.hpp")

#undef STAGE_ATTRIBUTE

  StageHelpers::DoubleLinkedList<Actor> _visibleActors;

  void Play(Scene *sc);

  void switchScene(Scene *);
  void onResize();

  Stage(int width, int height, float scale = 1.0)
      : _scene(0x0), _viewportOrigin({0, 0}),
        _stageRect(
            {0, 0, static_cast<float>(width), static_cast<float>(-height)}),
        _viewportRect({0, 0, static_cast<float>(width) * scale,
                       static_cast<float>(height) * scale}),
        _stageWidth(width), _stageHeight(height), _play({0, NULL}),
        _backgroundColor(Color{0x00, 0x00, 0xAA, 0xff}),
        _borderColor(Color{0x00, 0x88, 0xff, 0xff}), _handle_TICKING(),
        _handle_DEAD(), _handle_TRANSFORMABLE(),
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

}; // namespace Stage

//==============================================================================
// Implementations
//==============================================================================
namespace Stage {

// Stage Implementation
//------------------------------------------------------------------------------
inline void Stage::Play(Scene *sc) {

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(_viewportRect.width, _viewportRect.height, _stageTitle);
  if (!IsWindowReady()) {
    throw "window not ready";
  }

  _stage = LoadRenderTexture(_stageWidth, _stageHeight);
  if (!IsRenderTextureReady(_stage)) {
    CloseWindow();
    throw "failed to create render texture";
  }

  _play.stage = this;
  switchScene(sc);

  while (!WindowShouldClose() && _scene != NULL) {

    if (IsWindowResized())
      onResize();

    // Put DeltaTime - Multiplyer into context
    _play.deltaTime = GetFrameTime();

    // TODO: Find a better way todo this
    // Remove all Actors, that have been killed in the last cycle.
    //    for (auto killed : _attributelists.at(DEAD))
    //      RemoveActorFromStage(killed);

    // TODO: Cram the current Actors-State somehow into _play;

    // Tick the Scene with the state crated by the last frame.
    if (!_scene->OnTick(_play)) {
      // If the scene should end, attempt a scene Switch instead of
      // continuing.
      switchScene(NULL);
      continue;
    }

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

#define STAGE_ATTRIBUTE(name) _handle_ ## name.clear();
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


} // namespace Stage

#endif
