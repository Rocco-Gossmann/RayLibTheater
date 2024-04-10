#ifndef SCENES_MAIN
#define SCENES_MAIN 1

#include "../stage/theater.h"
#include "../actors/debug.h"

namespace Scenes {

class MainScene : public Stage::Scene {

public:
    void OnStageDraw(Stage::Play) override;
    Stage::Scene *OnUnload(Stage::Play) override;
    void OnLoad(Stage::Play) override;

private:
    Actors::Debug theDebug;
    Stage::Core::DoubleLinkedList<Actor> theDebugsSpot;

};
} // namespace Scenes

#endif // !SCENES_MAIN
