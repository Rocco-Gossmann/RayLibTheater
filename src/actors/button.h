#include <functional>
#ifndef BUTTON_H
#define BUTTON_H 1

#include "../lib/RayTheater.hpp"
#include "mouse.h"
#include "raylib.h"

using namespace Theater;

namespace Actors {

class Button : public Actor,
               public ColliderRect,
               public Visible,
               public Ticking {

public:
  typedef std::function<void(int, Button *)> OnClickHandler;

  Button(int id, float x, float y, float w, float h, Mouse *m,
         OnClickHandler *onclick);

  Rectangle getRect() override;
  void OnDraw(Play) override;

  bool OnTick(Play) override;

private:
  int _id;
  Rectangle _drawRect;
  Color _drawColor;
  Mouse *_mouse;
  OnClickHandler *_handler;
};

} // namespace Actors

#endif // BUTTON_H
