#ifndef MOUSE_H
#define MOUSE_H

#include "lib/RayTheater.hpp"
#include "raylib.h"

using namespace Theater;

namespace Actors {

class Mouse : public Actor, public Visible {
public:
  Mouse();

private:
  Texture2D gfx;
  bool cursorVisible;

public:
  // Implement Visible
  //---------------------------------------------------------------------------
  void OnDraw(Play) override;

private:
  // Implement Actor
  //---------------------------------------------------------------------------
  void OnStageEnter(Play) override;
  void OnStageLeave(Play) override;
};

} // namespace Actors

#endif
