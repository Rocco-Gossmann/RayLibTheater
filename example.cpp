#include "RayTheater.hpp"
#include <climits>
#include <cstdlib>
#include <ctime>
#include <raylib.h>

//==============================================================================
// NOTE: First lets create an actor to play the Mouse
// Since it draws to the Stage, we give it the Visible - Component.
// Since it needs to interact with other elements, we need to describe
// its shape for the collision. We use ColliderPoint for that.
//
// BM: Mouse
//==============================================================================
class Mouse : public Theater::Actor,
              public Theater::Visible,
              public Theater::ColliderPoint {

public:
  Mouse() : Theater::Actor(), Theater::Visible(this) {
    // to make sure the cursor is drawn above everything,
    // give it the highest possible Render-Layer
    this->SetRenderLayer(INT_MAX);
  }

private:
  Vector2 _pos;
  bool _click;

  // BM: Mouse - Implement Actor
  //---------------------------------------------------------------------------
  void OnStageEnter(Theater::Play p) {
    // Make mouse visible, as soon as it enters the stage
    p.stage->MakeActorVisible(this);
  }

  void OnStageLeave(Theater::Play p) {
    // If the Mouse leaves the stage, make it invisible
    // This is not needed, as a removed actor also looses its Visiblilaity
    // for the sake of this demo, this is just here, to demostrate, that Actors
    // can be made invisible, without having to remove it from the Stage
    p.stage->MakeActorInvisible(this);
  }

  // BM: Mouse - Implement Visible
  void OnDraw(Theater::Play p) {
    _pos = p.mouseLoc;
    _click = p.mouseDown > 0;
    DrawRectangle(p.mouseX - 4, p.mouseY - 4, 8, 8, RED);
  }

  // BM: Mouse - Implement ColliderPoint
  Vector2 getPosition() { return _pos; }
};

//==============================================================================
// NOTE: Then let's create an Actor to play the Target. It will be a square
// shape, It interacts with the Mouse, so we use a ColliderRect-Component to
// describe Its shape to the Collision check.
// --
// The Scene can also define, where it is located, from the outside
// so we add a Transform2D Component as well
// --
// Of course we need the Visible component, to make it visible on the Stage
//
// BM: Exit Button
//==============================================================================
class Target : public Theater::Actor,
               public Theater::Visible,
               public Theater::ColliderRect,
               public Theater::Transform2D {
public:
  Target()
      : Theater::Actor(), Theater::Transform2D(this), Theater::Visible(this) {}

private:
  // BM: Target - Implement ColliderRect
  //---------------------------------------------------------------------------
  // NOTE: Implementing getRect from Theater::ColliderRect
  // this will also be used to define where to draw the Rect
  Rectangle getRect() {
    auto pos = getLoc();
    return {pos.x, pos.y, 32, 32};
  }

  // BM: Target - Implement Visible
  //---------------------------------------------------------------------------
  // NOTE: To draw the target, we can use the CollisionRects coordinates and
  // size
  void OnDraw(Theater::Play p) { DrawRectangleRec(getRect(), GREEN); }
};

//==============================================================================
// NOTE: Now let's create the Scene to make use of these two actors.
//
// BM: Scene
//==============================================================================
class DemoScene : public Theater::Scene {

  // NOTE: first make sure our scene contains a Mouse- and a Target- Actor
private:
  Mouse mouse;
  Target target;

  // BM: Scene - Implement Scene
public:
  // NOTE: a Scene's OnStart Method is called once, before it starts playing
  void OnStart(Theater::Play p) {

    // We can use it to set up the random number generator
    std::srand(time(NULL));

    // and add our two actors to the Stage.
    p.stage->AddActor(&mouse);
    p.stage->AddActor(&target);

    // We'll also setup the Target, since it has a Transform2D Component, we can
    // change its location.
    target.setLoc({160, 120});
    p.stage->MakeActorVisible(&target);
  }

  // NOTE: a Scene's OnEnd Method is called once, after it finished playing
  // We'll remove our two actors from the Stage here
  void OnEnd(Theater::Play p) {
    p.stage->RemoveActor(&target);
    p.stage->RemoveActor(&mouse);
  }

  // NOTE: a Scene's OnUpdate Method is called each Cycle
  void OnUpdate(Theater::Play p) {

    if (p.mouseReleased > 0 && mouse.isCollidingWithRect(&target)) {
      // if the target was clicked => change the Targets Position to a random
      // location

      Vector2 pos = {
          static_cast<float>((double)(std::rand()) / (double)RAND_MAX) *
              (p.stageWidth - 32),
          static_cast<float>((double)(std::rand()) / (double)RAND_MAX) *
              (p.stageHeight - 32)};

      std::cout << "pos: " << pos.x << " - " << pos.y << std::endl;

      target.setLoc(pos);
    }
  }
};

//==============================================================================
// NOTE: Finally, let's use the Main-Function to Build the Stage and start
// playing the Scene
//
// BM: Mouse
//==============================================================================
int main(int argc, char **argv) {

  DemoScene sc;

  Theater::Builder(480, 320, 2)
      .Title("RayLib - Window 🎉")
      .Play((Theater::Scene *)&sc);

  return 0;
}
