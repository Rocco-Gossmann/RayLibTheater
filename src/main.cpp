#include "./lib/RayTheater.hpp"

#include "./scenes/scene_circle_line_intersection.h"
#include "./scenes/scene_main.h"
#include "./scenes/scene_point_in_polyshape.h"

int main(int argc, char **argv) {

  Scenes::CircleLineIntersectionScene scCircleLineIntersection;
  Scenes::PointInPolyShapeScene scPips;

  Scenes::MainScene scMain(&scCircleLineIntersection, &scPips);

  Theater::Builder(480, 320, 2)
      .Title("RayLib - Window 🎉")
      .Play((Scene *)&scMain);

  return 0;
}
