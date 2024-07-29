# Actors

An Actor can be any class, that implements the `Theater::Actor` interface.

# Example

```c++
#include "RayTheater.h"
#include <ralib.h>

using namespace Theater;

class Mouse : public Actor {

public:
  // Constructor
  Mouse() : Actor() {}

private:
  // Optional Event-Handlers, that allow the actor to load and unload
  // further ressources, when it is added to the Stage.
  void OnEnter(Stage *) override {}
  void OnLeave(Stage *) override {}

};
```

# Ticks / Updates
By overriding the `OnTick` Method, the Actor can be updated each frame/cycle.

```c++
  void OnTick(Stage * stage, Play play) override {}
```

# Visibility
To define how an Actor is rendered, you can overrid the Methods:

```c++
...
  void OnStageDraw() override {}
  void OnWindowDraw() override {}
...
```

However, an Actors visibility is defined by where it is positioned on the Stage.
The Stage consists of 256 layers. (0 - 255) the higher the number, the further in the back the Actor will be.
All Actors on layer 0 are not rendered though. (Consider it the area behind the curtain.)


# Loading and Unloading of Ressources

RayTheater makes use of RayLib for interacting with the rest of your Computer.
Therefore all functions provided by it are also Available to the Actor.

That is also, why you can't load Ressources inside the Constructor, because RayLib
needs to construct the Stage first.

This is where the Actors

```c++
  virtual void OnEnter(Stage *);
  virtual void OnLeave(Stage *);
```

methods come into play.

# A full Example for an Actor
Let's complete the Actor from the example code above.

```c++
#include "RayTheater.h"
#include <ralib.h>

using namespace Theater;

class Mouse : public Actor {

public:
  Mouse() : Actor() {}

private:

  // defining a space for the loaded image to live in Memory
  Texture2D m_gfx; //Texture2D is provided by RayLib

  // lets also define some coordinates, that the cursor is rendered at
  float m_posx, m_posy;

  // Loading the image as soon, as the Actor is added to the Stage.
  void OnEnter(Stage * stage) override {
    m_gfx = LoadTexture("./assets/cursor.png"); // RayLib function
  }

  // Once the Actor is removed from the Stage, we can free the Ressource
  void OnLeave(Stage *s) override {
    UnloadTexture(m_gfx); // RayLib function
  }

  // We need to update the  coordinates. 
  // Only during the Tick phase, will you get access 
  // to the Stage and the current Play (aka. the Stages state)
  void OnTick(Stage * stage, Play play) override {
 
    // Make sure cursor is still on the stage, when rendering it
    if(play.mouseX > 0 
      && play.mouseY > 0 
      && play.mouseX < play.stageWidth 
      && play.mouseY < play.stageHeight
    ) {
      m_posx = play.mouseX;      
      m_posy = play.mouseY;
    }

  }

  // Then we can draw the cursor
  void OnStageDraw() override {
      DrawTexture(m_gfx, m_posx, m_posy, WHITE); // RayLib function
  }
};
```

# Adding the Actor to the Stage

An Actor is not processed in an way, until it enters the Stage.

For it to enter though, the Actor should be part of a [Theater::Scene](./scenes.md):


```c++

#include "RayTheater.h"
#include <ralib.h>

using namespace Theater;

class Mouse /*...*/;
// ...


class MyScene : public Stage::Scene {
public:
  void OnLoad(Stage *s) override { 
    // Adding the Actor to the stage, as soon as it is loaded
    // m_mouseID is the registration for this actor
    // we need it to be able to remove the actor later 
    m_mouseID = s->AddActor(&m_mouse, /* layer: */ 1); //<- adding mouse on layer 1 means, 
                                          // it will always be rendereed above
                                          // everything else
  };

  void OnUnload(Stage *s) override {
    // Once the Scene is finished, we can unload the actor, by removing it's 
    // Registration
    s->RemoveActor(m_mouseID);
  }

private:
  // Holds the Actor itself
  Mouse m_mouse;

  // Upon adding the actor to the stage, A registration/ressource is created 
  // We need to keep track of it as well, so we can remove the actor from the stage 
  // later
  ActorRessource m_mouseID;

}
```

