#include "scenes/scene_main.h"
#include "raylib.h"
#include <algorithm>
#include <iostream>

using namespace Theater;

namespace Scenes {

//==============================================================================
// Implement Theater::Scene
//==============================================================================
void MainScene::OnLoad(Play p) {
  std::cout << "Scene load" << std::endl;
  p.stage->AddActor(&theDebug);
  p.stage->AddActor(&mousePtr);
}

Scene *MainScene::OnUnload(Play p) {
  std::cout << "Scene unload" << std::endl;
  p.stage->RemoveActor(&mousePtr);
  p.stage->RemoveActor(&theDebug);
  return NULL;
}

bool MainScene::OnTick(Play p) {

  auto lst = p.stage->GetActorsWithAttribute(MOUSEPTR);
  auto ptr = *lst.begin();

  return true;
}

void MainScene::OnStageDraw(Play p) {

  float cx = p.mouseLoc.x;
  float cy = p.mouseLoc.y;
  float cr = 20;

  float x1 = 10;
  float x2 = 60;
  float y1 = 20;
  float y2 = 50;

  DrawCircle(cx, cy, cr, BLUE);

  DrawLine(x1, y1, x2, y2, RED);

  float lineX = x2 - x1;
  float lineY = y2 - y1;
  float lineL = lineX * lineX + lineY * lineY;

  float circleX = cx - x1;
  float circleY = cy - y1;

  float t = fmax(0, fmin(lineL, (lineX * circleX + lineY * circleY))) / lineL;
  // float t = (lineX * circleX + lineY * circleY) / lineL;

  float offsetX = x1 + ((x2 - x1) * t);
  float offsetY = y1 + ((y2 - y1) * t);

  Color col = GREEN;

  if (((cx - offsetX) * (cx - offsetX) + (cy - offsetY) * (cy - offsetY)) <=
      (cr * cr))
    col = RED;

  DrawLine(offsetX, offsetY, cx, cy, col);

  float dy = (cy - y1);

  //  float dot = (((cx - x1) * (x2 - x1)) * ((cy - y1) * (y2 - y1))) / pow(ll,
  //  2);

  //  float clx = x1 + (dot * (x2 - x1));
  //  float cly = y1 + (dot * (y2 - y1));

  //  DrawLine(cx, cy, clx, cly, YELLOW);
}

} // namespace Scenes
