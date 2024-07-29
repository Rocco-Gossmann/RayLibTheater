#ifdef TESTS
#define RT_MAX_STAGE_ACTOR_COUNT 2

#include <RayTheater.hpp>

using namespace Theater;

class MyScene : public Stage::Scene {

public:
  MyScene() : m_timer_finished(false) {
    m_timer.setDuration(3000)
        ->onProgress(1000,
                     [this](Timer *, unsigned int pass, unsigned int goal) {
                       DebugLog("Timer-Progress: " << pass << " of " << goal);
                     })
        ->onFinish([this](Timer *) {
          DebugLog("Timer-Finished");
          m_timer_finished = true;
        });
  }

  void OnLoad(Stage *s) override { s->StartTimer(&m_timer); }

  void OnTick(Stage *s, Play p) override {
    if (m_timer_finished)
      s->EndPlay();
  }

private:
  Timer m_timer;
  bool m_timer_finished;
};

int main(int argc, char *argv[]) {

  MyScene s;
  DebugLog("--- Timers - Test ------------------------------ ");
  Builder(256, 192, 2).Title("Test 5 - Timers")->Play(&s);

  return 0;
}

#endif
