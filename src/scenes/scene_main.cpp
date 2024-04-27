#include "scenes/scene_main.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace Theater;
using namespace Theater::UI;

namespace Scenes {

MainScene::MainScene(CircleLineIntersectionScene *scli)
    : backBtn(INT_MAX, 262, 168, 48, 24), OnButton(NULL), activeScene(NULL) {

  OnButton = [this](int id, Button::ButtonEvent e, Button *btn) {
    if (e == Theater::UI::Button::BTN_RELEASE) {
      activeScene = this->buttonDef[id].scene;
      this->TransitionTo(activeScene);
    }
  };

  OnBack = [this](int id, Button::ButtonEvent _1, Button *_2) {
    if (activeScene != NULL) {
      activeScene->TransitionTo(this);
      activeScene = NULL;
    }
  };

  buttonDef[0].scene = scli;

  backBtn.Label("back")->OnRelease(&OnBack);
}

//==============================================================================
// Implement Theater::Scene
//==============================================================================
void MainScene::OnLoad(Play p) {
  std::cout << "Scene load" << std::endl;
  p.stage->AddActor(&mousePtr);
  p.stage->RemoveActor(&backBtn);

  for (int a = 0; a < MAINSCENE_BUTTON_ARR_CNT; a++) {
    auto def = buttonDef[a];
    buttons[a] = new Button(a, def.x, def.y, def.w, def.h);
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

  p.stage->AddActor(&backBtn);

  return SubScene::OnUnload(p);
}

} // namespace Scenes
