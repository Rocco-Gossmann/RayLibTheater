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
  void OnStageEnter(Play p) override {}
  void OnStageLeave(Play p) override {}

};
```

# Adding Components

To add Components, RayTheater makes use of C++ ability to use Multiple Inheritance.
Each component must be given the `this` in the Constructor.

So to allow the Above Actor to become visible on the Stage, we can add the `Theater::Visible` component.

```c++
#include "RayTheater.h"
#include <ralib.h>

using namespace Theater;

class Mouse : public Actor, public Visible {

public:
  // Constructor
  Mouse() : Actor(), Visible(this) {}
  // ...
};
```

From there, the Actors gains and exposes a new Method, for others or itself to use.

```c++
#include "RayTheater.h"
#include <ralib.h>

using namespace Theater;

class Mouse : public Actor, public Visible {

public:
  // ...
  /**
   * @brief Sets what elements are drawn above which other
   * actors with higher numbers will be drawn above actors with lower numbers
   * Should 2 elements have the same number, there draw order
   * depends on the order, in which they were made visible
   *
   * @param layer
   */
  void SetRenderLayer(int layer);
  // ...
};
```

It also receives a new Event-Handler, that gets triggered, when ever the Stage is Drawn.

```c++
#include "RayTheater.h"
#include <ralib.h>

using namespace Theater;

class Mouse : public Actor, public Visible {

private:
  // ...
  void OnDraw(Play p) override {};
};
```

# Loading and Unloading of Ressources

RayTheater makes use of RayLib for interacting with the rest of your Computer.
Therefore all functions provided by it are also Available to the Actor.

That is also, why you can't load Ressources inside the Constructor, because RayLib
needs to construct the Stage first.

This is where the Actors

```c++
  virtual void OnStageEnter(Play);
  virtual void OnStageLeave(Play);
```

methods come into play.

Lets update the Actor above to load a cursor Graphic and render it,

```c++
#include "RayTheater.h"
#include <ralib.h>

using namespace Theater;

class Mouse : public Actor, public Visible {

public:
  Mouse() : Actor(), Visible(this) {}

private:

  // defining a space for the loaded image to live in Memory
  Texture2D gfx; //Texture2D is provided by RayLib

  // Loading the image as soon, as the Actor is added to the Stage.
  void OnStageEnter(Play p) override {
    gfx = LoadTexture("./assets/cursor.png"); // RayLib function

    // Tell the Stage, that the Actor is ready to be rendered
    p.stage->MakeActorVisible(this);

  }

  // Once the Actor is removed from the Stage, we can free the Ressource
  void OnStageLeave(Play p) override {
    UnloadTexture(gfx); // RayLib function
  }

  // Then we can implement the Theater::Visible Interfaces  Hook to draw the cursor
  void OnDraw(Play p) override {
    // If the Cursor is inside the Stage
    if (p.mouseX > 0 && p.mouseY > 0 && p.mouseX < p.stageWidth &&
        p.mouseY < p.stageHeight) {

      DrawTexture(gfx, p.mouseX, p.mouseY, WHITE); // RayLib function

    }

  }
};
```
