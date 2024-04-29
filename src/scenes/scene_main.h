#ifndef SCENES_MAIN_H
#define SCENES_MAIN_H

#include "lib/RayTheater.hpp"
#include "lib/RayTheaterUI.hpp"
using namespace Theater::UI;

#include "./scene_circle_line_intersection.h"
#include "actors/mouse.h"

using namespace Theater;
using namespace Theater::UI;

namespace Scenes {

#define MAINSCENE_BUTTON_ARR_CNT 2

class MainScene : public Scene {
public:
  MainScene(CircleLineIntersectionScene *);

private:
  struct SceneButton {
    float w;
    std::string label;
    Scene *scene;
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
  Label versionLabel;
  Scene *activeScene;

  //==============================================================================
  // Implement Stage:Stage:::Scene
  //==============================================================================
public:
  void OnStart(Play p) override;
  void OnEnd(Play) override;
};

} // namespace Scenes

#endif // SCENES_MAIN_H
