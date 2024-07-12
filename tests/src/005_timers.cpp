#ifdef TESTS
#define RT_MAX_STAGE_ACTOR_COUNT 2

#include <RayTheater.hpp>

using namespace Theater;

class MyScene : public Stage::Scene {

private:
  Timer m_timer;
};

int main(int argc, char *argv[]) {

  MyScene s;
  DebugLog("--- Timers - Test ------------------------------ ");
  Builder(256, 192, 2).Title("Test 5 - Timers")->Play(&s);

  return 0;
}

#endif
