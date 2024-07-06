#ifndef RAYTHEATER_ACTOR_H
#define RAYTHEATER_ACTOR_H

#include "./RayTheaterAttributes.hpp";
#include "./RayTheaterPlay.h";
#include <unordered_set>

namespace Theater {

class ActorComponent;

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

} // namespace Theater
#endif // RAYTHEATER_ACTOR_H