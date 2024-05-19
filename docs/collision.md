# Theater::Colliders

Colliders are means for Actors to interact on the stage with each other.
Please note, that Colliders are ***not concidered [ActorComponents](./components.md)***. 
yet an [Actor](./actors.md) an still inherit from it. 
(As the example In [Theater::Components](./components.md) shows)

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

