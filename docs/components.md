# Actor - Components

An [Actor](./actors.md) by itself is essentially an empty Object.

You can add and remove it to and from the stage as well as add Attributes. However, its true potential can only be unlocked by giving it Components.

# How to add Components to Actors

For adding components to Actors, RayTheater makes use of C++'s ability to let Classes inherit from multiple Parents.

An Example probably shows best, how this works.

```c++
#include "RayTheater.hpp"

// Create an Actor to play the Mouse Cursor
class Mouse : public Theater::Actor,        // Actor - base class
              public Theater::Visible,      // ActorComponent
              public Theater::ColliderPoint 
{
public:
    Mouse() : Theater::Actor(), Theater::Visible(this) {}

private:
    Vector2 _mousePos;

    // Implement Theater::ColliderPoint
    Vector2 getPosition() override { return _mousePos; }

    // Implement Theater::Visible
    void OnDraw(Theater::Play p) override {
        _mousePos = p.mouseLoc;
        /* ... Call some RayLib function to draw this */
    };

    // Implement Theater::Actor
    void OnStageEnter(Theater::Play p) override {
        p.stage->MakeActorVisible(this);
    }

}
```

### 1. Let your Actor inherit from [Theater::Actor](./actors.md) and from any Theater::Compontent class you like.

```c++
class Mouse : public Theater::Actor,
              public Theater::Visible,
              // ...
```

 `Theater::Actor` gives it Attributes and basic Actor Functionality.
 `Theater::Visible` tells the Engine, that this is something, that can be drawn to the screen.

### 2. Then give the Actor a shape, by adding a [`Theater::Collider`](./collision.md)
```c++
class Mouse : public Theater::Actor,
              public Theater::Visible,
              public Theater::ColliderPoint { //<--
// ...
```
`Theater::ColliderPoint` Allows other Colliders Attribute to check they are overlapping with the Mouse's current position.

### 3. You need to call the Actors Constructor, As well as the Compontents::Constructor. Each Components Constructor must be given the Actors reference. You can use `this` here.

```c++
public:
    Mouse() : Theater::Actor(),
              Theater::Visible(this) {}
```

3.) Implement what ever virtual functions the Components define.
In this case:

- `Visible` requires a `OnDraw` Method, that gets called, when ever the Actor is Drawn to the Stage. All Virtual Methods of components should be private.

- `ColliderPoint` requires a `getPosition` Method, that points to the current position of the Point, that should be checked for overlaps.

```c++
private:
    // Implement Theater::ColliderPoint
    Vector2 getPosition() override { return _mousePos; }

    // Implement Theater::Visible
    void OnDraw(Theater::Play p) override {
        _mousePos = p.mouseLoc;
        /* ... Call some RayLib function to draw this */
    };
```

4.) make the Mouse is visible as soon as it is added to the Stage. For that we can override the optional ` OnStageEnter ` Hook of the Actor-Class. 
```c++
// Implement Theater::Actor
void OnStageEnter(Theater::Play p) override {
    p.stage->MakeActorVisible(this);
}
```

# Visible - Component

As shown in the beginning example, this component is needed, to render an Actor on the Stage.

Actors are not visible by default to make an Actor with a Visible - Component visible or invisible on the Stage, you need to call the Stages methods.
```c++
  bool MakeActorVisible(Theater::Visible *);
  // and 
  void MakeActorInvisible(Theater::Visible *);
```

### virtual Methods:
```c++
/** @brief called, once it is time to render the Actor to the Screen
 * Within OnDraw, you are drawing on the Stage. 
 * The stages Pixel-Dimensions are provided via:
 * 
 * @param p - Contains meta data about the Stage and current User-Inputs
 */
void OnDraw(Theater::Play p) override;
```

# Ticking - Component

A Ticking - Component is invoked every cycle (similar to a visible [Visible - Component](#visible---component) )
The Playback of a Ticking - Component  can be Paused and Continued via the following [Stage's](./stage.md) Methods 

```c++
  bool Pause();
  // and 
  void UnPause();
```

### virtual Methods:
```c++
/** @brief called, Each cycle, as long as the Scene is not Paused
 * @param p - Contains meta data about the Stage and current User-Inputs
 */
void OnTick(Theater::Play p) override;
```

# Transform2D - Component
[!WIP]
This component provides a set of functions, that allows other Actors and Scense
to manipulate it's transformation from the outside.

### Methods Provided to other Actors

```c++
  /** @return privides the Actors location, at the star of the cycle */
  Vector2 getLoc(); 


  /** @brief Requests the Actor to change its location at the beginning
   * of the next Cycle
   * @param l the new Location, the Actor should move to
   */
  void setLoc(Vector2 l);
```

> [!NOTE]  
> new methods, for transforming `rotation` and `scale` may come in the future


