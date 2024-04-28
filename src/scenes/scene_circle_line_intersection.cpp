#include "./scene_circle_line_intersection.h"

#include "raylib.h"
#include <cmath>

void Scenes::CircleLineIntersectionScene::OnStageDraw(Theater::Play p) {

  float cx = p.mouseLoc.x;
  float cy = p.mouseLoc.y;
  float cr = 20;

  float x1 = 120;
  float x2 = 240;
  float y1 = 80;
  float y2 = 120;

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
}
