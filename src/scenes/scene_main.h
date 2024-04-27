#ifndef SCENES_MAIN_H
#define SCENES_MAIN_H

#include "lib/RayTheater.hpp"
#include "lib/RayTheaterUI.hpp"

#include "./scene_circle_line_intersection.hpp"
#include "actors/debug.h"
#include "actors/mouse.h"
#include "subscene.h"

using namespace Theater;
using namespace Theater::UI;

namespace Scenes {

#define MAINSCENE_BUTTON_ARR_CNT 2

class MainScene : public SubScene {
public:
  MainScene(CircleLineIntersectionScene *);

private:
  struct SceneButton {
    float x, y, w, h;
    std::string label;
    SubScene *scene;
  };

  Button::ButtonEventHandler OnButton;
  Button::ButtonEventHandler OnBack;

  Actors::Mouse mousePtr;

  SceneButton buttonDef[MAINSCENE_BUTTON_ARR_CNT] = {
      {8, 8, 128, 24, "Circle Line - Collision", NULL},
      {8, 34, 64, 24, "Exit", NULL},
  };

  Button *buttons[MAINSCENE_BUTTON_ARR_CNT];
  Button backBtn;
  SubScene *activeScene = NULL;

  //==============================================================================
  // Implement Stage:Stage:::Scene
  //==============================================================================
public:
  void OnLoad(Play p) override;
  Theater::Scene *OnUnload(Play) override;
};

} // namespace Scenes

#endif // SCENES_MAIN_H
