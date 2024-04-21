#include "scenes/scene_main.h"
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

  p.stage->MakeActorVisible(&mousePtr);
}

Scene *MainScene::OnUnload(Play p) {
  std::cout << "Scene unload" << std::endl;
  p.stage->RemoveActor(&mousePtr);
  p.stage->RemoveActor(&theDebug);
  return NULL;
}

bool MainScene::OnTick(Play p) { return true; }

} // namespace Scenes
