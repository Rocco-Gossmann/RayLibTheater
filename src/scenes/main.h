#ifndef SCENES_MAIN
#define SCENES_MAIN 1

#include "../stage/scene.h"

namespace Scenes {
    class MainScene : public Stage::Scene {

    public:
        void OnStageDraw(Stage::Play);
        Stage::Scene* OnUnload(Stage::Play);
        void OnLoad(Stage::Play);

    };
}


#endif // !SCENES_MAIN
