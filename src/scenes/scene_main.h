#ifndef SCENES_MAIN_H
#define SCENES_MAIN_H

#include "lib/RayTheater.hpp"
#include "lib/RayTheaterUI.hpp"

#include "actors/debug.h"
#include "actors/mouse.h"

using namespace Theater;
using namespace Theater::UI;

namespace Scenes {

#define MAINSCENE_BUTTON_ARR_CNT 2

class MainScene : public Scene {
public:
  MainScene();

private:
  struct SceneButton {
    int id;
    float x, y, w, h;
    std::string label;
  };

  Button::ButtonEventHandler OnButton;

  Actors::DebugActor theDebug;
  Actors::Mouse mousePtr;

  SceneButton buttonDef[MAINSCENE_BUTTON_ARR_CNT] = {
      {1, 8, 8, 64, 24, "Btn 1"},
      {1, 76, 8, 64, 24, "Btn 2"},
  };

  Button *buttons[MAINSCENE_BUTTON_ARR_CNT];

  //==============================================================================
  // Implement Stage:Stage:::Scene
  //==============================================================================
public:
  void OnLoad(Play p) override;
  Theater::Scene *OnUnload(Play) override;
  bool OnTick(Play p) override;
};

} // namespace Scenes

#endif // SCENES_MAIN_H
