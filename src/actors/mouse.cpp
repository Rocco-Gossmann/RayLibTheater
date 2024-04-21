#include "./mouse.h"
#include "raylib.h"

#include <iostream>
using namespace std;
using namespace Theater;

namespace Actors {

Mouse::Mouse() : Actor(), Ticking(this), Transform2D(this), Visible(this){};

bool Mouse::OnTick(Theater::Play p) {
  cout << " mouse ticking => Update position " << endl;
  setLoc(p.mouseLoc);
  return true;
}

void Mouse::OnDraw(Theater::Play p) {
  auto loc = getLoc();
  cout << " TODO: render mouse cursor at " << loc.x << " - " << loc.y << endl;
}

} // namespace Actors
