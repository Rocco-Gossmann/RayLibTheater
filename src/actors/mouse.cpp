#include "./mouse.h"
#include "raylib.h"

#include <iostream>
using namespace std;
using namespace Theater;

namespace Actors {

Mouse::Mouse() : Actor(), Ticking(this), Transform2D(this){};

bool Mouse::OnTick(Theater::Play p) {

  auto loc = getLoc();
  cout << " mouse ticking: " << p.mouseX << " - " << p.mouseY << endl;

  setLoc(p.mouseLoc);

  return true;
}

} // namespace Actors
