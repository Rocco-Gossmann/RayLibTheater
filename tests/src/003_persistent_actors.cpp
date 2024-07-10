#ifdef TESTS

#define RT_MAX_STAGE_ACTOR_COUNT 1

#include "RayTheater.hpp"
#include "internal/macros.h"

using namespace Theater;

//==============================================================================
// BM: DebugActor - keept in global scope for simplicity
//==============================================================================
class DebugActor : public Actor {
public:
  void OnEnter(Stage *) override { DebugLog("[DebugActor] onEnter"); }
  void OnTick(Stage *, Play) override { DebugLog("[DebugActor] onTick"); }
  void OnLeave(Stage *) override { DebugLog("[DebugActor] onLeave"); }

  ActorRessource stageID;
};

DebugActor da;

//==============================================================================
// BM: Scenes
//==============================================================================
class MySecondScene : public Stage::Scene {
  void OnLoad(Stage *s) override { DebugLog("Followup loaded"); };
  void OnTick(Stage *s, Play p) override { s->EndPlay(); }
  void OnUnload(Stage *s) override {
    s->RemoveActor(da.stageID);
    s->RemoveActor(da.stageID);
  }
};

class MyScene : public Stage::Scene {
public:
  MyScene() : Stage::Scene(), m_followup(nullptr) {}
  MyScene(MySecondScene *sc = nullptr) : Stage::Scene(), m_followup(sc) {}

  void OnLoad(Stage *s) override { da.stageID = s->AddActor(&da); };
  void OnTick(Stage *s, Play p) override { s->ChangeScene(m_followup); };

private:
  MySecondScene *m_followup = nullptr;
};

int main() {

  MySecondScene s2;
  MyScene s(&s2);

  DebugLog("--- Persistent Actors - Test ------------------------------ ");
  Builder(256, 192, 2).Title("Test 3 - Persistent Actors")->Play(&s);

  return 0;
}

#endif
