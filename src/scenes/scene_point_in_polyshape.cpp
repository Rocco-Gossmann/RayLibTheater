#include "scene_point_in_polyshape.h"
#include "raylib.h"
#include <cstdio>
#include <vector>

Scenes::PointInPolyShapeScene::PointInPolyShapeScene() : dots({}) {}

void Scenes::PointInPolyShapeScene::OnStart(Theater::Play p) { dots.clear(); }

void Scenes::PointInPolyShapeScene::OnUpdate(Theater::Play p) {
  // Place new Point with Left Button
  if ((p.mouseDown & (1 << 1)) > 0) {
    dots.push_back(Vertex(p.mouseLoc));
    return;
  }

  // Remove Point with Right Button
  if ((p.mouseDown & (1 << 2)) > 0) {
    for (int a = 0; a < dots.size(); a++) {
      auto dot = dots.at(a);

      if (dot.containsPoint(p.mouseLoc)) {
        dots.erase(dots.begin() + a);
        break;
      }
    }
    return;
  }
}

void Scenes::PointInPolyShapeScene::OnStageDraw(Theater::Play p) {

  // Draw Dots
  for (auto dot : dots) {
    auto dot1 = dot.getPosition();
    DrawCircleLines(dot1.x, dot1.y, dot.getRadius(), WHITE);
  }

  // Draw Helper-Lines for Mouse Position
  DrawLine(p.mouseX, p.mouseY, p.stageWidth, p.mouseY, YELLOW);

  unsigned int intersections = 0;

  // If there is at least 2 dots, draw the lines of the shape.
  if (dots.size() > 1) {
    for (int a = 0; a < dots.size() - 1; a++)
      intersections += connectDots(a, a + 1, p.mouseLoc);

    intersections += connectDots(dots.size() - 1, 0, p.mouseLoc);
  }

  std::cout << "Intersections: " << intersections << std::endl;

  if ((intersections & 1) == 1) {
    DrawText("mouse In shape", 160, 8, 20, GREEN);
  }
}

void Scenes::PointInPolyShapeScene::OnWindowDraw(Theater::Play p) {

  char *str;

  std::snprintf(str, 64, "DOWN: %d\nHELD: %d\nUP: %d\n", p.mouseDown,
                p.mouseHeld, p.mouseUp);

  DrawText(str, 8, 8, 20, GREEN);
}

unsigned int Scenes::PointInPolyShapeScene::connectDots(int d1, int d2,
                                                        Vector2 mouse) {

  unsigned int ret = 0;

  auto pos1 = dots.at(d1).getPosition();
  auto pos2 = dots.at(d2).getPosition();
  Color c = WHITE;

  unsigned char b1 =
      ((pos1.y <= mouse.y ? 1 : 0) << 0) | ((pos1.y > mouse.y ? 1 : 0) << 1);

  unsigned char b2 =
      ((pos2.y <= mouse.y ? 1 : 0) << 0) | ((pos2.y > mouse.y ? 1 : 0) << 1);

  if ((b1 ^ b2) == 3) {
    float lineX = pos2.x - pos1.x;
    float lineY = pos2.y - pos1.y;
    float lineL = lineX * lineX + lineY * lineY;

    float circleX = mouse.x - pos1.x;
    float circleY = mouse.y - pos1.y;

    float t = fmax(0, fmin(lineL, (lineX * circleX + lineY * circleY))) / lineL;

    float offsetX = pos1.x + ((pos2.x - pos1.x) * t);
    float offsetY = pos1.y + ((pos2.y - pos1.y) * t);

    if (offsetX > mouse.x) {
      DrawCircle(offsetX, offsetY, 10, YELLOW);
      DrawLine(offsetX, offsetY, mouse.x, mouse.y, GREEN);
      c = RED;
      ret = 1;
    }
  }

  DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, c);

  return ret;
}
