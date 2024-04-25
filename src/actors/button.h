#ifndef BUTTON_H
#define BUTTON_H 1

#include "../lib/RayTheater.hpp"
#include "raylib.h"

using namespace Theater;

namespace Actors {

class Button : public Actor, public ColliderRect, public Visible {
public:
  Button(float x, float y, float w, float h);

  Rectangle getRect() override;
  void OnDraw(Play) override;

  void OnStageEnter(Play) override;
  void OnStageLeave(Play) override;

private:
  Rectangle _drawRect;
  Color _drawColor;
};

} // namespace Actors

#endif // BUTTON_H
