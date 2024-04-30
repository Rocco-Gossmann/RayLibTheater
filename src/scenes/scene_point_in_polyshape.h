
#include "raylib.h"
#include <vector>
#ifndef SCENE_POINT_IN_POLYSHAPE_H
#define SCENE_POINT_IN_POLYSHAPE_H 1

#include "../lib/RayTheater.hpp"

using namespace Theater;

namespace Scenes {

class PointInPolyShapeScene : public Scene {

public:
  PointInPolyShapeScene();

private:
  class Vertex : public ColliderCircle {
  public:
    Vertex(Vector2 pos) : pos(pos) {}
    Vector2 getPosition() { return pos; };
    float getRadius() { return 8; }

  private:
    Vector2 pos;
  };

  std::vector<Vertex> dots;

  unsigned int connectDots(int d1, int d2, Vector2 mouse);

public:
  //==============================================================================
  // BM: Implement Scene
  //==============================================================================
  void OnStart(Play) override;
  void OnStageDraw(Play) override;
  void OnWindowDraw(Play) override;
  void OnUpdate(Play) override;
};

} // namespace Scenes
#endif // SCENE_POINT_IN_POLYSHAPE_H
