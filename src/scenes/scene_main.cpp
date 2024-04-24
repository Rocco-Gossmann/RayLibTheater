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

} // namespace Scenes
