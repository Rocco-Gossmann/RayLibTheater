#ifdef TESTS

#define RT_MAX_STAGE_ACTOR_COUNT 2

#include <string>
#include <unordered_set>

#include "RayTheater.hpp"

using namespace Theater;

//==============================================================================
// BM: DebugActor - Class
//==============================================================================
class DebugActor : public Theater::Actor {
public:
  Theater::AttributeList GetAttributes() override {
    return Theater::AttributeList{Theater::DEBUG_ITEM};
  }

  Theater::ActorRessource m_id;
  std::string m_label;
};

//==============================================================================
// BM: Scene - Class
//==============================================================================
class MyScene : public Theater::Stage::Scene {
public:
  void OnLoad(Theater::Stage *s) override {
    m_debugActor.m_label = "Hello";
    m_debugActor.m_id = s->AddActor(&m_debugActor);
    s->AddActorAttribute(m_debugActor.m_id, Theater::NPC);

    m_debugActor1.m_label = "World";
    m_debugActor1.m_id = s->AddActor(&m_debugActor1);
    s->AddActorAttribute(m_debugActor1.m_id, Theater::NPC);
  };

  void OnTick(Theater::Stage *s, Theater::Play p) override {
    auto lst = s->getActorsWithAttribute(Theater::DEBUG_ITEM);

    auto si = lst.size();

    switch (si) {
    case 2:
      for (auto a : lst) {
        DebugLog("Found: " << ((DebugActor *)a)->m_label);
        if (a->HasAttribute(Theater::NPC))
          DebugLog("is NPC");
      }

      s->RemoveActor(m_debugActor.m_id);
      break;

    case 1:
      for (auto a : lst)
        if (a->HasAttribute(Theater::NPC)) {
          DebugLog("Left with NPC DebugActor: " << ((DebugActor *)a)->m_label
                                                << " => removing NPC Tag");
          s->RemoveActorAttribute(((DebugActor *)a)->m_id, Theater::NPC);
        } else {
          DebugLog("Last Debug Actor has no NPC Tag => Remove from Stage");
          s->RemoveActor(m_debugActor1.m_id);
        }
      break;

    case 0:
      DebugLog("No actors on stage => End Programm");
      s->EndPlay();
      break;
    default:
      assert(false && "list with unexpected length");
    }
  }

  void OnUnload(Theater::Stage *s) override {
    s->RemoveActor(m_debugActor.m_id);
    s->RemoveActor(m_debugActor1.m_id);
  };

private:
  DebugActor m_debugActor;
  DebugActor m_debugActor1;
};

//==============================================================================
// BM: Main
//==============================================================================
int main() {
  MyScene s;
  DebugLog("--- Actor - Attributes - Test ------------------------------ ");
  Theater::Builder(256, 192, 2).Title("Test 4 - Actor - Attributes")->Play(&s);

  return 0;
}

#endif
