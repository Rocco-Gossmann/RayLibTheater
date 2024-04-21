#ifndef MOUSE_H
#define MOUSE_H

#include "lib/RayTheater.hpp"

using namespace Theater;

namespace Actors {

class Mouse : public Actor, public Ticking, public Transform2D, public Visible {
public:
  Mouse();
  bool OnTick(Play) override;
  void OnDraw(Play) override;
};

} // namespace Actors

#endif
