#ifndef SCENES_MAIN_H
#define SCENES_MAIN_H

#include "lib/RayTheater.hpp"
#include "lib/RayTheaterUI.hpp"

#include "actors/debug.h"
#include "actors/mouse.h"

using namespace Theater;

namespace Scenes {

class MainScene : public Scene {
public:
  MainScene();

private:
  Theater::UI::Button::ButtonEventHandler OnButton;

  Actors::DebugActor theDebug;
  Actors::Mouse mousePtr;
  Theater::UI::Button btn1;
  Theater::UI::Button btn2;

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
