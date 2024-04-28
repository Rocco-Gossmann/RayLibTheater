#ifndef SCENES_MAIN_H
#define SCENES_MAIN_H

#include "lib/RayTheater.hpp"
#include "lib/RayTheaterUI.hpp"

#include "./scene_circle_line_intersection.hpp"
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
    float w;
    std::string label;
    SubScene *scene;
  };

  Button::ButtonEventHandler OnButton;
  Button::ButtonEventHandler OnBack;
  Button::UIStyle _btnStyle;

  Actors::Mouse mousePtr;

  SceneButton buttonDef[MAINSCENE_BUTTON_ARR_CNT] = {
      {128, "Circle Line - Collision", NULL},
      {64, "Exit", NULL},
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
