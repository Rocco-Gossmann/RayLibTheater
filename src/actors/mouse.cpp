#include "./mouse.h"
#include "raylib.h"

using namespace std;
using namespace Theater;

namespace Actors {

Mouse::Mouse() : Actor(), Visible(this), cursorVisible(true){};

void Mouse::OnDraw(Theater::Play p) {

  if (p.mouseX > 0 && p.mouseY > 0 && p.mouseX < p.stageWidth &&
      p.mouseY < p.stageHeight) {

    if (cursorVisible) {
      HideCursor();
      cursorVisible = false;
    }
    DrawTexture(gfx, p.mouseX, p.mouseY, WHITE);

  } else if (!cursorVisible) {
    ShowCursor();
    cursorVisible = true;
  }
}

void Mouse::OnStageEnter(Theater::Play p) {
  gfx = LoadTexture("./assets/cursor.png");
}

void Mouse::OnStageLeave(Theater::Play p) {
  UnloadTexture(gfx);
  ShowCursor();
}

} // namespace Actors
