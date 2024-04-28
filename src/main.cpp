#include "./lib/RayTheater.hpp"

#include "./scenes/scene_circle_line_intersection.hpp"
#include "./scenes/scene_main.h"

int main(int argc, char **argv) {

  Scenes::CircleLineIntersectionScene scCircleLineIntersection;

  Scenes::MainScene scMain(&scCircleLineIntersection);

  Theater::Builder(480, 320, 2)
      .Title("RayLib - Window 🎉")
      .Play((Scene *)&scMain);

  return 0;
}
