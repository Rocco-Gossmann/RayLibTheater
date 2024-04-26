#ifndef MOUSE_H
#define MOUSE_H

#include "lib/RayTheater.hpp"
#include "raylib.h"

using namespace Theater;

namespace Actors {

class Mouse : public Actor,
              public Ticking,
              public Visible,
              public ColliderPoint {
public:
  Mouse();
  Vector2 getPosition() override;
  bool clicked();

private:
  Texture2D gfx;
  bool cursorVisible;
  Vector2 _pos;
  bool _clicked;

private:
  // Implement Visible
  //---------------------------------------------------------------------------
  void OnDraw(Play) override;

  // Implement Ticking
  //---------------------------------------------------------------------------
  bool OnTick(Play) override;

  // Implement Actor
  //---------------------------------------------------------------------------
  void OnStageEnter(Play) override;
  void OnStageLeave(Play) override;
};

} // namespace Actors

#endif
