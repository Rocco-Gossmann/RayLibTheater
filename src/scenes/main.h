#ifndef SCENES_MAIN
#define SCENES_MAIN 1

#include "lib/RayTheater.hpp"

#include "../actors/debug.h"

namespace Scenes {

class MainScene : public Stage::Scene {

public:
    void OnStageDraw(Stage::Play);
    Stage::Scene *OnUnload(Stage::Play);
    void OnLoad(Stage::Play);

private:
    Actors::Debug theDebug;

};
} // namespace Scenes

#endif // !SCENES_MAIN
