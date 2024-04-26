#include "button.h"
#include "mouse.h"
#include "raylib.h"

using namespace Theater;

namespace Actors {

Button::Button(int id, float x, float y, float w, float h, Mouse *m,
               Button::OnClickHandler *onclick)
    : Actor(), Visible(this), Ticking(this), _id(id), _drawRect({x, y, w, h}),
      _drawColor(BLUE), _mouse(m), _handler(onclick) {}

void Button::OnDraw(Play p) { DrawRectangleRec(_drawRect, _drawColor); }

Rectangle Button::getRect() { return _drawRect; }

bool Button::OnTick(Play p) {

  if (isCollidingWithPoint(_mouse) && _mouse->clicked()) {
    _drawColor = RED;
    if (_handler != NULL) {
      (*_handler)(_id, this);
    }
  } else {
    _drawColor = BLUE;
  }

  return true;
}

} // namespace Actors
