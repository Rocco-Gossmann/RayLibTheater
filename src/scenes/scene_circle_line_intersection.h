
#ifndef SCENE_CIRCLE_LINE_INTERSECTION_H
#define SCENE_CIRCLE_LINE_INTERSECTION_H 1
#include "../lib/RayTheater.hpp"
#include "./subscene.h"

using namespace Theater;

namespace Scenes {

class CircleLineIntersectionScene : public SubScene {
public:
  void OnStageDraw(Play p) override;
};

} // namespace Scenes

#endif // SCENE_CIRCLE_LINE_INTERSECTION_H
