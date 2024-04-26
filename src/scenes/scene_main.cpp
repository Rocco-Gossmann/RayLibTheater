#include "scenes/scene_main.h"
#include <iostream>

using namespace Theater;

namespace Scenes {

MainScene::MainScene()
    : OnButton([](int id, UI::Button::ButtonEvent e, UI::Button *btn) {
        std::cout << " Hit Button (" << e << "): " << id << std::endl;
      }),
      btn1(1, 8, 8, 64, 24), btn2(2, 8 + 68, 8, 64, 24) {
  btn1.Label("BTN 1")
      ->OnHover(&OnButton)
      ->OnPress(&OnButton)
      ->OnHold(&OnButton)
      ->OnRelease(&OnButton);

  btn2.Label("BTN 2")
      ->OnHover(&OnButton)
      ->OnPress(&OnButton)
      ->OnHold(&OnButton)
      ->OnRelease(&OnButton);
}

//==============================================================================
// Implement Theater::Scene
//==============================================================================
void MainScene::OnLoad(Play p) {
  std::cout << "Scene load" << std::endl;
  p.stage->AddActor(&mousePtr);
  p.stage->AddActor(&btn1);
  p.stage->AddActor(&btn2);
}

Scene *MainScene::OnUnload(Play p) {
  std::cout << "Scene unload" << std::endl;
  p.stage->RemoveActor(&btn1);
  p.stage->RemoveActor(&btn2);
  return NULL;
}

bool MainScene::OnTick(Play p) { return true; }

} // namespace Scenes
