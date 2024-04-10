#include "scene.h"

namespace Stage {

    void Scene::OnLoad(Play) { }
    Scene* Scene::OnUnload(Play) { return NULL; }
    void Scene::OnStageDraw(Play) { }
    void Scene::OnWindowDraw(Play) { }

    bool Scene::OnTick(Play) { return false; }
    void Scene::OnStageRemove(Play) {}
}

