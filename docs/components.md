# Actor - Components

An [Actor](./actors.md) by itself is essentially an empty Object.

You can add and remove it to and from the stage as well as add Attributes. However, its true potential can only be unlocked by giving it Components.

# How to add Components to Actors

For adding components to Actors, RayTheater makes use of C++'s ability to let Classes inherit from multiple Parents.

An Example probably shows best, how this works.

```c++
#include "RayTheater.hpp"

// Create an Actor to play the Mouse Cursor
class Mouse : public Theater::Actor,
              public Theater::Visible,
              public Theater::ColliderPoint
{
public:
    Mouse() : Theater::Actor(), Theater::Visible(this), Theater::ColliderPoint(this) {}

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

1.) Let your Actor inherit from [Theater::Actor](./actors.md) and from any Theater::Compontent class you like.

```c++
class Mouse : public Theater::Actor,
              public Theater::Visible,
              public Theater::ColliderPoint
```

- `Theater::Actor` gives it Attributes and basic Actor Functionality.
- `Theater::Visible` tells the Engine, that this is something, that can be drawn to the screen.
- `Theater::ColliderPoint` Allows other Actors with `COLLIDER` Attribute to check they are overlapping with the Mouse's current position.

  2.) You need to call the Actors Constructor, As well as the Compontents::Constructor. Each Components Constructor must be given the Actors reference. You can use `this` here.

```c++
public:
    Mouse() : Theater::Actor(),
              Theater::Visible(this),
              Theater::ColliderPoint(this) {}
```

3.) Implement what ever virtual function the Components define.
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

This component provides a set 


# Collider - Components

Colliders can be represented by various shapes, from a single Point `ColliderPoint`, over a Circle `ColliderCircle` or a Rectangle `ColliderRect`, to full shapes drawn by a path with multiple points. `ColliderZone`

Add adding a Collider - Component, your Actor must also Implement it's Virtual methods.  
These Methods are then used to describe the Colliders exact shape.


## Theater::ColliderPoint
Points these colliders consist of a single Pixel. 
It is there for best used for a mouse.

### virtual Methods:
```c++
/** @brief Must provid the position of the Pixel that the collider occupies right now.  */
Vector2 getPosition() override;
```

## Theater::ColliderCircle
Circle Colliders are similar to Point Colliders.
But instead of a single Pixel, the span a circluar area around the given point as well.

### virtual Methods:
```c++
/** @brief 
@return the position of the Pixel that the collider occupies right now.  
*/
Vector2 getPosition() override;

/** @brief
@return The radius of the circular area around the point provided by
getPosition 
*/
float getRadius() override;
```

## Theater::ColliderRect
Rectangular Colliders are basically just a rectangular box
Not much to say here.

### virtual Methods:
```c++
/** @brief 
ˇA rectangle representing the collision area *//
Rectangle getRect() override;
```


## Theater::ColliderZone
Zones are the most complex form to check collisions with.
As such, using them in access may impact your performance.

A zone is defined by a C++'s `std::vector` of RayLibs `Vector2`s 
Each Point/`Vector2` you define is connected to the Point/`Vector2` that comes before it.  The Last `Vector2` in your list automatically 
connected back to the first `Vector2` of your list.

By drawing a line from point to point, the outline of your CollisionZone is defined.


### virtual Methods:
```c++
/** @return the list of points, that together draw the outline
 of the zone */
std::vector<Vector2> *getZoneBorder()  override;
```