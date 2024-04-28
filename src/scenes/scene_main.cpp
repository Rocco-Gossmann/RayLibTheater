#include "scenes/scene_main.h"
#include "raylib.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace Theater;
using namespace Theater::UI;

namespace Scenes {

MainScene::MainScene(CircleLineIntersectionScene *scli)
    : backBtn(INT_MAX, 262, 168, 48, 24), OnButton(NULL), activeScene(NULL) {

  // Set Button Style
  _btnStyle.roundTL = false;
  _btnStyle.roundBR = false;
  _btnStyle.cornorRadius = 0.5;
  _btnStyle.labelOffset.x = 8;
  _btnStyle.labelOffset.y = 8;

  // Hook up the SubScene
  buttonDef[0].scene = scli;

  // Define Button Action
  OnButton = [this](int id, Button::ButtonEvent _1, Button *_2) {
    activeScene = this->buttonDef[id].scene;
    this->TransitionTo(activeScene);
  };

  // Define Button Action
  OnBack = [this](int id, Button::ButtonEvent _1, Button *_2) {
    if (activeScene != NULL) {
      activeScene->TransitionTo(this);
      activeScene = NULL;
    }
  };

  // Configure Backbutton
  backBtn.Label("back")->Style(&_btnStyle)->OnRelease(&OnBack);
}

//==============================================================================
// Implement Theater::Scene
//==============================================================================
void MainScene::OnLoad(Play p) {

  SetExitKey(KEY_NULL);
  p.stage->AddActor(&mousePtr);

  // When ever this Scene is loaded, make sure
  // the Back-Button is not visible
  p.stage->RemoveActor(&backBtn);

  // Create Buttons for all configured Scenes
  // the Back-Button is not visible
  int y = 8;
  for (int a = 0; a < MAINSCENE_BUTTON_ARR_CNT; a++, y += 28) {
    auto def = buttonDef[a];
    buttons[a] = new Button(a, 8, y, def.w, 24);
    buttons[a]->Label(def.label)->Style(&_btnStyle)->OnRelease(&OnButton);
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
