#include "scene.h"
#include "entity.h"

namespace Stage {

    Scene::Scene() : Entity() { }

    void Scene::OnLoad(Play) { }
    Scene* Scene::OnUnload(Play) { return NULL; }
    bool Scene::OnTick(Play) { return false; }
    void Scene::OnStageDraw(Play) { }
    void Scene::OnWindowDraw(Play) { }

    void Scene::OnStageRemove(Play) {}
}

