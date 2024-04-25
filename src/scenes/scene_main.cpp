#include "scenes/scene_main.h"
#include <iostream>

using namespace Theater;

namespace Scenes {

MainScene::MainScene() : btn1(8, 8, 64, 24) {}

//==============================================================================
// Implement Theater::Scene
//==============================================================================
void MainScene::OnLoad(Play p) {
  std::cout << "Scene load" << std::endl;
  p.stage->AddActor(&mousePtr);
  p.stage->AddActor(&btn1);
  p.stage->MakeActorVisible(&btn1);
}

Scene *MainScene::OnUnload(Play p) {
  std::cout << "Scene unload" << std::endl;
  p.stage->RemoveActor(&btn1);
  return NULL;
}

bool MainScene::OnTick(Play p) { return true; }

} // namespace Scenes
