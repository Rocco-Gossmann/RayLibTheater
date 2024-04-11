#ifndef RAYTHEATER_H
#define RAYTHEATER_H 1

#include <raylib.h>
#include <unordered_map>
#include <unordered_set>

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

//=============================================================================
// Stage::Attributes
//=============================================================================
enum Attributes {
#define STAGE_ATTRIBUTE(name) name,

#if __has_include("RayTheaterAttributes.h")
#include "RayTheaterAttributes.h"
#endif // __has_include("RayTheaterAttributes.hpp")

  STAGE_ATTRIBUTE(TICKING) STAGE_ATTRIBUTE(DEAD)
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
/**
 * @class Actor
 * @brief An Entity, that can act while a Stage::Scene is playing
 * various of its functions get invoced during the play.
 */
class Actor {
  friend class Stage;

public:
  virtual bool OnTick(Play);
  virtual void OnStageRemove(Play);

  bool hasAttribute(Attributes attr);

private:
  std::unordered_set<Attributes> _attributes;

  void addAttribute(Attributes a);
  void removeAttribute(Attributes a);
};

//=============================================================================
// Stage::Scene
//=============================================================================
class Scene : public Actor {
public:
  virtual void OnLoad(Play);
  virtual Scene *OnUnload(Play);
  virtual void OnStageDraw(Play);
  virtual void OnWindowDraw(Play);

  virtual bool OnTick(Play) override;
  virtual void OnStageRemove(Play) override;
};

//=============================================================================
// Stage::Stage
//=============================================================================
class Stage {
  friend class Builder;

public:
  ~Stage();

  Stage BorderColor(Color);
  Stage BackgroundColor(Color);

  /**
   * @brief Adds an actor to a group with a given Attribute
   *
   * TICKING => The Actors OnTick method will be invoked each frame
   * DEAD => Actor Will be removed from all Groups before next Draw
   *
   * give an Actor the DEAD Attribute to remove it from all lists
   *
   * You can also add other Attributes via the "EntityAttributes.hpp"
   */
  void AddActorAttribute(Actor *, Attributes);

  /**
   * @brief Removes an Attribute from a given Actor
   */
  void RemoveActorAttribute(Actor *, Attributes);

  void MakeActorVisible(Actor *);

  void MakeActorInvisible(Actor *);

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

  std::unordered_map<Attributes, std::unordered_set<Actor *>> _attributelists;

  StageHelpers::DoubleLinkedList<Actor> _visibleActors;

  Stage(int width, int height, float scale = 1.0);
  void Play(Scene *sc);

  void RemoveActorFromStage(Actor *);
  void switchScene(Scene *);
  void onResize();
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
  Builder(int width, int height, float scale = 1.0);

  Builder Title(const char *);

  Builder BorderColor(Color);
  Builder BackgroundColor(Color);

  void Play(Scene *);

private:
  Stage _stage;
};

//==============================================================================
// Implementations
//==============================================================================
#ifdef RAYTHEATER_MAIN

// Builder Implementation
//------------------------------------------------------------------------------
inline Builder::Builder(int width, int height, float scale)
    : _stage(Stage(width, height, scale)) {}

inline Builder Builder::Title(const char *t) {
  _stage._stageTitle = t;
  return *this;
}

inline Builder Builder::BorderColor(Color c) {
  _stage._borderColor = c;
  return *this;
}

inline Builder Builder::BackgroundColor(Color c) {
  _stage._backgroundColor = c;
  return *this;
}

inline void Builder::Play(Scene *sc) { _stage.Play(sc); }

// Default Scene Implementation
//------------------------------------------------------------------------------
inline void Scene::OnLoad(Play) {}
inline Scene *Scene::OnUnload(Play) { return NULL; }
inline void Scene::OnStageDraw(Play) {}
inline void Scene::OnWindowDraw(Play) {}

inline bool Scene::OnTick(Play) { return false; }
inline void Scene::OnStageRemove(Play) {}

// Default Actor Implementation
//------------------------------------------------------------------------------
inline bool Actor::OnTick(Play) { return false; }
inline void Actor::OnStageRemove(Play) {}

inline void Actor::addAttribute(Attributes a) { _attributes.insert(a); }

inline void Actor::removeAttribute(Attributes a) {
  auto attr = this->_attributes.find(a);

  if (attr != _attributes.end()) {
    _attributes.erase(a);
  }
}

inline bool Actor::hasAttribute(Attributes a) {
  return _attributes.find(a) != _attributes.end();
}

// Stage Implementation
//------------------------------------------------------------------------------
#include <cstdio>

inline Stage::Stage(const int width, const int height, float scale)
    : _scene(0x0), _viewportOrigin({0, 0}),
      _stageRect(
          {0, 0, static_cast<float>(width), static_cast<float>(-height)}),
      _viewportRect({0, 0, static_cast<float>(width) * scale,
                     static_cast<float>(height) * scale}),
      _stageWidth(width), _stageHeight(height), _play({0, NULL}),
      _backgroundColor(Color{0x00, 0x00, 0xAA, 0xff}),
      _borderColor(Color{0x00, 0x88, 0xff, 0xff}),
      _stageTitle("< RayWrapC - Project >") {

#define STAGE_ATTRIBUTE(name)                                                 \
  _attributelists.insert(                                                      \
      {name, ::std::unordered_set<Actor *>()});
#if __has_include("RayTheaterAttributes.h")
#include "RayTheaterAttributes.h"
#endif // __has_include("RayTheaterAttributes.hpp")
  STAGE_ATTRIBUTE(TICKING) STAGE_ATTRIBUTE(DEAD)
#undef STAGE_ATTRIBUTE
}

inline Stage::~Stage() { _scene = NULL; }

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

    // TODO: Update delta time in _play;

    // Tick all the actors
    for (auto ticker : _attributelists.at(TICKING)) {
      if (!ticker->OnTick(_play)) {
        AddActorAttribute(ticker, DEAD);
      }
    }

    // if the scene is over, switch to the next scene
    if (_scene->hasAttribute(DEAD)) {
      switchScene(NULL);
      continue;
    } else {
      // Otherwise Remove all who should no longer act in the Scene
      for (auto killed : _attributelists.at(DEAD)) {
        RemoveActorFromStage(killed);
      }
    }

    BeginTextureMode(_stage);
    ClearBackground(_backgroundColor);
    _scene->OnStageDraw(_play);
    EndTextureMode();

    BeginDrawing();

    ClearBackground(_borderColor);

    DrawTexturePro(_stage.texture, _stageRect, _viewportRect, _viewportOrigin,
                   0, WHITE);

    _scene->OnWindowDraw(_play);

    EndDrawing();
  }

  switchScene(0);
  UnloadRenderTexture(_stage);
}

inline void Stage::switchScene(Scene *sc) {
  if (_scene != NULL) {
    RemoveActorFromStage(_scene);

    if (sc == NULL)
      sc = _scene->OnUnload(_play);
    else
      _scene->OnUnload(_play);

    _scene = NULL;
  }

  if (sc != NULL) {
    sc->OnLoad(_play);
    _scene = sc;
    RemoveActorAttribute(_scene, DEAD);
    AddActorAttribute(_scene, TICKING);
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

inline void Stage::AddActorAttribute(Actor *a, Attributes at) {
  if (a == NULL || a->hasAttribute(at))
    return;
  a->addAttribute(at);
  _attributelists.at(at).insert(a);
}

inline void Stage::RemoveActorAttribute(Actor *a, Attributes at) {
  a->removeAttribute(at);

  auto al = _attributelists.at(at);
  auto ac = al.find(a);
  if (ac != al.end())
    al.erase(ac);
}

inline void Stage::RemoveActorFromStage(Actor *a) {

#define STAGE_ATTRIBUTE(name) RemoveActorAttribute(a, name);
#if __has_include("RayTheaterAttributes.h")
#include "RayTheaterAttributes.h"
#endif // __has_include("RayTheaterAttributes.hpp")
  STAGE_ATTRIBUTE(TICKING) 
    STAGE_ATTRIBUTE(DEAD)
#undef STAGE_ATTRIBUTE
}

inline void Stage::MakeActorVisible(Actor *a) {
  if (_visibleActors.Find(a) == 0)
    _visibleActors.Push(a);
}

inline void Stage::MakeActorInvisible(Actor *a) {
  StageHelpers::DoubleLinkedList<Actor> *lst;
  while ((lst = _visibleActors.Find(a)) != 0)
    lst->Drop();
}

#endif

} // namespace Stage
#endif // RAYTHEATER_H
