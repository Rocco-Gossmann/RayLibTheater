// The user can stop this example from being compiled, by
#ifndef DISABLE_RAYTHEATER_EXAMPLE

#include "RayTheater.hpp"
#include "RayTheaterCollider.hpp"
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <raylib.h>

//==============================================================================
// NOTE: First lets create an actor to play the Mouse
// Since it draws to the Stage, we give it the Visible - Component.
// Since it needs to interact with other elements, we need to describe
// its shape for the collision. We use ColliderPoint for that.
//
// BM: Mouse Actor
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

  // BM: Mouse Actor - Implement Actor
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

  // BM: Mouse Actor - Implement Visible
  void OnDraw(Theater::Play p) {
    _pos = p.mouseLoc;
    _click = p.mouseDown > 0;
    DrawRectangle(p.mouseX - 4, p.mouseY - 4, 8, 8, RED);
  }

  // BM: Mouse Actor - Implement ColliderPoint
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
// BM: Target Actor - Class
//==============================================================================
class Target : public Theater::Actor,
               public Theater::Visible,
               public Theater::ColliderRect,
               public Theater::Transform2D {
public:
  Target()
      : Theater::Actor(), Theater::Transform2D(this), Theater::Visible(this) {}

private:
  // BM: Target Actor - Implement ColliderRect
  //---------------------------------------------------------------------------
  // NOTE: Implementing getRect from Theater::ColliderRect
  // this will also be used to define where to draw the Rect
  Rectangle getRect() {
    auto pos = getLoc();
    return {pos.x, pos.y, 32, 32};
  }

  // BM: Target Actor - Implement Visible
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

  // The Scene will also display some helpfull messages, in case the user/player
  // takes to long, we'll use a Timer for that.
  Theater::Timer helpTimer;
  bool m_showHelp = false;
  int m_timerProgress = 0;

  // BM: Scene - Implement Scene
public:
  // Note: In the Scenes constructor, wen can set up the timer that delays
  // the display of the helpfull message. We can also set up other things, that
  // will never change over the applications life time. (like the seed for rand)
  DemoScene() {
    helpTimer
        .setDuration(5000 /* the timer keeps running for 5 Seconds */)

        // When it finishes it toggles the help message to be visible
        ->onFinish([this](Theater::Timer *t) { this->m_showHelp = true; })

        // We want to keep track of how long the timer has been running for
        // So we can instruct the timer to call a different function every
        // 1000ms
        ->onProgress(1000,
                     [this](Theater::Timer *t, float progress, float goal) {
                       m_timerProgress = static_cast<int>(progress);
                     });

    // Lets also set up the random number generator here.
    std::srand(time(NULL));
  }

  // NOTE: a Scene's OnStart Method is called once, before it starts playing
  void OnStart(Theater::Play p) override {

    // Lets add our two actors to the stage
    p.stage->AddActor(&mouse);
    p.stage->AddActor(&target);

    // Unlike the mouse, the `target` does not become visible on its own.
    // It also needs some further setting up, so lets do that here.
    // change its location.
    target.setLoc({160, 120});
    // and make it visible
    p.stage->MakeActorVisible(&target);

    // at last start the timer, we prepared in the constructor
    p.stage->SetTimer(&helpTimer);
  }

  // NOTE: a Scene's OnEnd Method is called once, after it finished playing
  // We'll remove our two actors and the timer from the Stage again
  void OnEnd(Theater::Play p) override {
    p.stage->RemoveActor(&target);
    p.stage->RemoveActor(&mouse);
    p.stage->RemoveTimer(&helpTimer);
  }

  // NOTE: a Scene's OnUpdate Method is called each Cycle
  void OnUpdate(Theater::Play p) override {

    // Here we can handle some simple application logic
    if (p.mouseDown > 0 && mouse.isCollidingWithRect(&target)) {

      // if the target was clicked => change the Targets Position to a random
      // location
      Vector2 pos = {
          static_cast<float>((double)(std::rand()) / (double)RAND_MAX) *
              (p.stageWidth - 32),
          static_cast<float>((double)(std::rand()) / (double)RAND_MAX) *
              (p.stageHeight - 32)};

      std::cout << "new target pos: " << pos.x << " - " << pos.y << std::endl;
      target.setLoc(pos);

      // We'll also reset the HelpMessage and restart the timer for displaying
      // it
      m_showHelp = false;
      p.stage->SetTimer(&helpTimer);
    }
  }

  // Note: a Scene's Stage Draw methods is called onced per cycle too, but
  // its can directly influence what is displayed on the stage area.
  void OnStageDraw(Theater::Play p) override {
    if (m_showHelp) {

      // Draw the help message, if it is visible
      DrawText(" click the green box :-) ", 8, 8, 10, WHITE);
    } else {

      // if the help-message is not visible yet, show how many seconds have
      // passed
      char buffer[256];
      std::snprintf(buffer, 256, "%d", m_timerProgress / 1000);
      DrawText(buffer, 8, 8, 10, WHITE);
    }
  }
};

//==============================================================================
// NOTE: Finally, let's use the Main-Function to Build the Stage and start
// playing the Scene
//
// BM: Main
//==============================================================================
int main(int argc, char **argv) {

  DemoScene sc;

  Theater::Builder(480, 320, 2)
      .Title("RayLib - Window 🎉")
      .IntegerScale()
      .Play((Theater::Scene *)&sc);

  return 0;
}

#endif
