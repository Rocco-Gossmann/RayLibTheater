#include "./lib/RayTheater.hpp"

#include "./scenes/scene_main.h"

int main(int argc, char **argv) {

  Scenes::MainScene scMain;

  Theater::Builder(320, 200, 2.0)
      .Title("RayLib - Window 🎉")
      .Play((Scene *)&scMain);

  return 0;
}
