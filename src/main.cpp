#include "raylib.h"
#include <iostream>
#define RAYTHEATER_MAIN
#include "./lib/RayTheater.hpp"

#include "./actors/debug.h"

class MainScene : public Stage::Scene {
public:
  void OnLoad(Stage::Play p) {
    std::cout << "Scene load" << std::endl;
    p.stage->AddActorAttribute(&theDebug, Stage::TICKING);
  }

  Stage::Scene *OnUnload(Stage::Play p) {
    std::cout << "Scene unload" << std::endl;
    p.stage->RemoveActorAttribute(&theDebug, Stage::TICKING);
    return NULL;
  }

  bool OnTick(Stage::Play p) { return true; }

private:
  Actors::DebugActor theDebug;

};



int main(int argc, char **argv) {

  MainScene scMain;

  Stage::Builder(320, 200, 2.0)
    .Title("RayLib - Window 🎉")
    .Play(&scMain);

  return 0;
}
