#include "main.h"
#include "raylib.h"
#include <iostream>

namespace Scenes {

void MainScene::OnStageDraw(Stage::Play p) {
    DrawText("Hello Stage !!!", 4, 4, 10, WHITE);
}

Stage::Scene* MainScene::OnUnload(Stage::Play p) {
    std::cout << "Scene unload" << std::endl;
    return NULL;
}

void MainScene::OnLoad(Stage::Play p) {
    std::cout << "Scene load" << std::endl;
}

}
