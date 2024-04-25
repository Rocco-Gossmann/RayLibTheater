#include "button.h"
#include "raylib.h"

using namespace Theater;

namespace Actors {

Button::Button(float x, float y, float w, float h)
    : Actor(), Visible(this), _drawRect({x, y, w, h}), _drawColor(BLUE) {}

void Button::OnDraw(Play p) { DrawRectangleRec(_drawRect, BLUE); }

Rectangle Button::getRect() { return _drawRect; }

void Button::OnStageEnter(Play p) {
  std::cout << "Button entered stage" << std::endl;
}

void Button::OnStageLeave(Play p) {
  std::cout << "Button entered stage" << std::endl;
}

} // namespace Actors
