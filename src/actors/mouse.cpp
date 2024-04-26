#include "./mouse.h"
#include "raylib.h"

using namespace std;
using namespace Theater;

namespace Actors {

Mouse::Mouse() : Actor(), Ticking(this), Visible(this), cursorVisible(true){};

bool Mouse::OnTick(Play p) {
  _pos.x = p.mouseX;
  _pos.y = p.mouseY;
  _clicked = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

  return true;
}

void Mouse::OnDraw(Theater::Play p) {

  if (_pos.x > 0 && _pos.y > 0 && _pos.x < p.stageWidth &&
      _pos.y < p.stageHeight) {

    if (cursorVisible) {
      HideCursor();
      cursorVisible = false;
    }
    DrawTexture(gfx, _pos.x, _pos.y, WHITE);

  } else if (!cursorVisible) {
    ShowCursor();
    cursorVisible = true;
  }
}

void Mouse::OnStageEnter(Theater::Play p) {
  gfx = LoadTexture("./assets/cursor.png");
  p.stage->AddActorAttribute(this, MOUSEPTR);
  this->SetRenderLayer(999);
  p.stage->MakeActorVisible(this);
}

void Mouse::OnStageLeave(Theater::Play p) {
  p.stage->RemoveActorAttribute(this, MOUSEPTR);
  UnloadTexture(gfx);
  ShowCursor();
}

Vector2 Mouse::getPosition() { return _pos; }
bool Mouse::clicked() { return _clicked; }

} // namespace Actors
