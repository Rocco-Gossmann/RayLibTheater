#include "scenes/scene_main.h"
#include <iostream>

using namespace Theater;
using namespace Theater::UI;

namespace Scenes {

MainScene::MainScene() : OnButton(NULL) {
  OnButton = [](int id, Button::ButtonEvent e, Button *btn) {
    std::cout << " Hit Button (" << e << "): " << id << std::endl;
  };
}

//==============================================================================
// Implement Theater::Scene
//==============================================================================
void MainScene::OnLoad(Play p) {
  std::cout << "Scene load" << std::endl;
  p.stage->AddActor(&mousePtr);

  for (int a = 0; a < MAINSCENE_BUTTON_ARR_CNT; a++) {
    auto def = buttonDef[a];
    buttons[a] = new Button(def.id, def.x, def.y, def.w, def.h);
    buttons[a]
        ->Label(def.label)
        ->LabelOffset(8, 8)
        ->OnEvent(&OnButton) // Setting all Event to be Handled
                             // by OnButton
        // Disableing Event-Handling for Hover and Hold Events
        ->OnHover(NULL)
        ->OnHold(NULL);

    p.stage->AddActor(buttons[a]);
  }
}

Scene *MainScene::OnUnload(Play p) {
  std::cout << "Scene unload" << std::endl;

  for (int a = 0; a < MAINSCENE_BUTTON_ARR_CNT; a++) {
    p.stage->RemoveActor(buttons[a]);
    delete buttons[a];
  }

  return NULL;
}

bool MainScene::OnTick(Play p) { return true; }

} // namespace Scenes
