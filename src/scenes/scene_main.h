#ifndef SCENES_MAIN_H
#define SCENES_MAIN_H

#include "lib/RayTheater.hpp"

#include "actors/button.h"
#include "actors/debug.h"
#include "actors/mouse.h"

using namespace Theater;

namespace Scenes {

class MainScene : public Scene {
public:
  MainScene();

private:
  Actors::Button::OnClickHandler OnButtonClick;

  Actors::DebugActor theDebug;
  Actors::Mouse mousePtr;
  Actors::Button btn1;
  Actors::Button btn2;

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
