#include "RayTheater.hpp"

using namespace Theater;

class MyScene : public Stage::Scene {};

int main() {
  const MyScene s;
  Builder(256, 192, 2).Title("Test 1 - Basics")->Play(&s);
}
