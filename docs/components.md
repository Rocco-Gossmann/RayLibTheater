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

# Colliders

Colliders can be represented by various shapes, from a single Point `ColliderPoint`, over a Circle `ColliderCircle` or a Rectangle `ColliderRect`, to full shapes drawn by a path with multiple points. `ColliderZone`

## Theater::ColliderPoint

[!todo]

## Theater::ColliderCircle

[!todo]

## Theater::ColliderRect

[!todo]

## Theater::ColliderZone

[!todo]
