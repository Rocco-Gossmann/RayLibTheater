#include "RayTheater.hpp"

using namespace Theater;

class MyScene : public Stage::Scene {
  void OnLoad(Stage *s) override {
    // For this test, it is enough, if the scene is loaded before it ends
    s->EndPlay();
  };
};

int main() {
  MyScene s;
  DebugLog("--- Base - Test ------------------------------ ");
  Builder(256, 192, 2).Title("Test 1 - Basics")->Play(&s);

  return 0;
}
