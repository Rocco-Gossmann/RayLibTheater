# Theater::Stage

The Stages primary purpouse is to proved access to all Actors currently active on the Stage.

# Access during Runtime

The stage can be accessed via the [`Theater::Play`s](./play.md) `stage` propperty given to any of a [Scenes](./scenes.md)

```c++
void OnStart(Thater::Play p);
void OnUpdate(Thater::Play p);
void OnStageDraw(Thater::Play p);
void OnWindowDraw(Thater::Play p);
void OnEnd(Thater::Play p);
```

Methods.

It is also part of the Events fired to Actors, that enter or leave the Stage:

```c++
void OnStageEnter(Play);
void OnStageLeave(Play);
```

# Accessable Functions

```c++
/** @brief Pauses all Ticking Actors */
void Pause();

/**  @brief Continues to run all Ticking Actors */
void UnPause();

/**
 * @brief Changes the Color of the Border, that is show, when the window is
 * scaled to an aspect ratio different, than the Stages
 */
void BorderColor(Color);

/**
 * @brief Background-Color of the stage. (Each frame, the stage is cleared
 * to this color.
 */
void BackgroundColor(Color);

/**
 * @brief Registers an Actor as being on the stage
 *
 * @tparam T any class that implement Theater::Actor
 * @param a - reference that the Stage will use to adress the Actor
 */
template <typename T> void AddActor(T *a);

/**
 * @brief Marks any Actor as "DEAD", meaning, it will be removed from the
 * stage between cycles
 *
 * @param a the pointer to the actor to remove (Must be the same value as
 * given in AddActor)
 */
void RemoveActor(Actor *a);

/**
 * @brief By default Actors are invisible / Not rendered
 * use this function to make it visible (Add it to the Stages render-list)
 *
 * @tparam T any class that implements Theater::Actor and Theater::Visible
 * @return true on success ; false == to many Actors are already on stage or
 * you called this function during rendering
 */
template <typename T> bool MakeActorVisible(T *);

/**
 * @brief removes any visible Actor from the stages render-list
 *
 * @tparam T any class that implements Theater::Visible
 */
template <typename T> void MakeActorInvisible(T *);

/**
 * @brief Allows for adding custom Attributes to an Actor on the Stage
 *
 * @return - true = attribute added; false = Actor not on stage or used
 * illegal Attribute
 */
bool AddActorAttribute(Actor *, Attributes);

/**
 * @brief Removes a custom Attribute from an Actor on the Stage
 *
 * @return - true = attribute removed; false = Actor not on stage or used
 illegal
 * Attribute

 */
bool RemoveActorAttribute(Actor *, Attributes);

/**
 * @brief gets an unordered_set of Actors all having the requested Attributes
 *
 * @param attr
 */
std::unordered_set<Actor *> GetActorsWithAttribute(Attributes attr);

```
