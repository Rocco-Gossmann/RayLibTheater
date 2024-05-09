---
id: custom_attributes
aliases: []
tags: []
---

# Attributes

Attributes can be seen as little "True" "False" Flags, that communicate
an Actors properties to to any other Actor and the Stage.

An Actors Attributes can only be manipulated through the Stage.
For example, if the Actor is added to or removed from the Stage.

```c++
  bool AddActorAttribute(Actor *, Attributes);

  bool RemoveActorAttribute(Actor *, Attributes);
```

# [!WARNING]

Custom Attributes can be a powerful tool.
But with that it is also your responsibility to manage how you want to use them.

# Example

Let's say we wan't an Actor, that plays a "Mouse-Cursor" in our Play.

In its draw Function, we update its Position like so:

```c++
void Mouse::OnDraw(Theater::Play play) {

  if (play.mouseX > 0 && play.mouseY > 0 && play.mouseX < play.stageWidth &&
      play.mouseY < play.stageHeight) {

    this->posX = play.mouseX
    this->posY = play.mouseY

    DrawTexture(gfx, p.mouseX, p.mouseY, WHITE);

  }
}
```

But we also want other actors to interact with it.
So we need a custom Attribute.
So somewhere in your compilers `include path`, put a `RayTheaterAttributes.hpp` into it.

Alternatively add the `-Ipath/to/the/folder/containing/RayTheaterAttributes.hpp` Flag
to your Compiler arguments. (more details on [RayTheaterAttrivute.hpp](./raytheater_attributes_hpp.md))

Now in the `RayTheaterAttributes.hpp` add the following Line.

```c++
STAGE_ATTRIBUTE(MOUSEPTR)
```

Then we can tell our Mouse to use the Attribute when it enters the stage.

```c++
void Mouse::OnStageEnter(Theater::Play p) {
  // ...

  p.stage->AddActorAttribute(this, MOUSEPTR);
}
```

And just to clean up well remove the Attribute, when the Mouse is removed from the Stage

```c++
void Mouse::OnStageLeave(Theater::Play p) {
  // ...

  p.stage->RemoveActorAttribute(this, MOUSEPTR);
}
```

Lets Say, we now wan't to get the Actor, that is currently registered as a `MOUSEPTR`.
We can simply request a list of Actors with the `MOUSEPTR` Attribute from the Stage.

```c++
auto lst = p.stage->GetActorsWithAttribute(MOUSEPTR);
if(lst.size > 0)
  auto ptr = *lst.begin();
```

Alternatively, if you know the Actor, you can also check if it was given a special Attribute.

For example, any Actor with a `DEAD` Attribute will be removed automatically from from the stage
before the next cycle will start.

```c++
Actor *a = //...

if( a->hasAttribute(DEAD) ) {
  // Do something, before the actor gets removed next cycle
}
```

## Internal Attributes

RayTheater comes with a group of predefined, internal Attributes.
Thease are

### DEAD

Any Actor with the `DEAD` Attribute will be removed from the Stage between cycles.

### VISIBLE

An Actor with the `VISIBLE` Attribute implements the `Theater::Visible` Interface and thus can be rendered to the Stage.
via the Stages

```c++
  Stage::MakeActorVisible(T *);
// and
  Stage::MakeActorInvisible(T *);
```

Methods

### TRANSFORMABLE

An Actor with the `TRANSFORMABLE` Attribute implements the `Theater::Transform2D` Interface and thus receives access to functions like `getLoc`(ation) and `setLoc`(ation)
Making it possible for others to influence the Actors transformations.

### TICKING

If an Actor has the `TICKING` Attribute, it runs it's own Update-routine once per cycle.
