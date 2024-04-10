#include <iostream>
#include <raylib.h>
#include "main.h"

namespace Scenes {

void MainScene::OnStageDraw(Stage::Play p) {
    DrawText("Hello Stage !!!", 4, 4, 10, WHITE);
}

Stage::Scene* MainScene::OnUnload(Stage::Play p) {
    std::cout << "Scene unload" << std::endl;

    p.stage->RemoveActorAttribute(&theDebug, Actor::TICKING);
    theDebugsSpot.Drop();

    return NULL;
}

void MainScene::OnLoad(Stage::Play p) {
    std::cout << "Scene load" << std::endl;
    p.stage->BorderColor(RED);


    p.stage->AddActorAttribute(&theDebug, Stage::Actor::TICKING);
    theDebugsSpot = p.stage->MakeActorVisible(&theDebug);

}

}
