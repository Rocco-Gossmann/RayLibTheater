#define RT_MAX_STAGE_ACTOR_COUNT 2

#include "RayTheater.hpp"

using namespace Theater;

//==============================================================================
// BM: DebugActor - Class
//==============================================================================

class DebugActor : public Actor {
public:
  DebugActor() : Actor(1) {}

  void OnEnter(Stage *) override { DebugLog("[DebugActor] onEnter"); }
  void OnTick(Stage *, Play) override { DebugLog("[DebugActor] onTick"); }
  void OnStageDraw() override { DebugLog("[DebugActor] onStageDraw"); }
  void OnWindowDraw() override { DebugLog("[DebugActor] onWindowDraw"); }
  void OnLeave(Stage *) override { DebugLog("[DebugActor] onLeave"); }
};

//==============================================================================
// BM: Scene - Class
//==============================================================================

class MyScene : public Stage::Scene {
public:
  void OnLoad(Stage *s) override {
    m_debugActor_id = s->AddActor(&m_debugActor);
    DebugLog("[MyScene::OnLoad] got Actor " << m_debugActor_id);
    m_debugActor_id2 = s->AddActor(&m_debugActor2);
    DebugLog("[MyScene::OnLoad] got Actor " << m_debugActor_id2);
  };

  void OnTick(Stage *s, Play p) override { s->EndPlay(); }

  void OnUnload(Stage *s) override {
    DebugLog("[MyScene::OnUnload] remove Actor " << m_debugActor_id);
    s->RemoveActor(m_debugActor_id);
    // Not remvoing m_debugActor_id2, to test if the automatic cleanup works
  };

private:
  DebugActor m_debugActor;
  DebugActor m_debugActor2;
  ActorRessource m_debugActor_id;
  ActorRessource m_debugActor_id2;
};

//==============================================================================
// BM: Main
//==============================================================================

int main() {
  MyScene s;
  DebugLog("--- Actors - Test ------------------------------ ");
  Builder(256, 192, 2).Title("Test 2 - Actors")->Play(&s);

  return 0;
}
