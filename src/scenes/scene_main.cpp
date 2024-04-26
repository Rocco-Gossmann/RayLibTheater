#include "scenes/scene_main.h"
#include <iostream>

using namespace Theater;

namespace Scenes {

MainScene::MainScene()
    : OnButtonClick([](int id, Actors::Button *btn) {
        std::cout << " Hit Button: " << id << std::endl;
      }),
      btn1(1, 8, 8, 64, 24, &mousePtr, &OnButtonClick),
      btn2(2, 8 + 68, 8, 64, 24, &mousePtr, &OnButtonClick) {}

//==============================================================================
// Implement Theater::Scene
//==============================================================================
void MainScene::OnLoad(Play p) {
  std::cout << "Scene load" << std::endl;
  p.stage->AddActor(&mousePtr);

  p.stage->AddActor(&btn1);
  p.stage->MakeActorVisible(&btn1);

  p.stage->AddActor(&btn2);
  p.stage->MakeActorVisible(&btn2);
}

Scene *MainScene::OnUnload(Play p) {
  std::cout << "Scene unload" << std::endl;
  p.stage->RemoveActor(&btn1);
  p.stage->RemoveActor(&btn2);
  return NULL;
}

bool MainScene::OnTick(Play p) { return true; }

} // namespace Scenes
