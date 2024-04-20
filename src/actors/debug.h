#ifndef ACTORS_DEBUG_H
#define ACTORS_DEBUG_H 1

#include "lib/RayTheater.hpp"

using namespace Theater;

namespace Actors {

class DebugActor : public Actor, public Transform2D, public Ticking {
public:
  DebugActor();

  bool OnTick(Play);
};

} // namespace Actors

#endif // ACTORS_DEBUG_H
