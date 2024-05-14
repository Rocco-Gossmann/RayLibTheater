# Theater::Scene

scenes are classes, that Inherit from the ` Theater::Scene ` base class.

Your first Scene must be passed to the [ Theater::Builders Play - Method](./builder.md#Play)

After that You can transition to other Scenes from within the current Scene, via the [Scene::TransitionTo](#scenetransitionto) method.

# Scene::TransitionTo

```c++
void TransitionTo(Theater::Scene *s)
```
This will transition to the next given Scene ` s `.

You can also pass in ` NULL ` / ` nullptr ` / ` 0 `.
This will Unload the current Scene and then Exit the Application.

So to end the Application, use
```c++
this->TransitionTo(NULL); 
``` 
in any of your Scenes [Live-Cycle-Hooks](#life-cycle-hooks).


# Life-Cycle Hooks

the `Theater::Scene` BaseClass comes with a few abstract methods, that can be overwritten to hook into various events , that RayTheater is running each Cycle.

## Scene::OnStart
```c++
void OnStart(Theater::Play p) override
```
Is called once before the Scene starts playing.
This is, where you add your Actors to the [Stage](./stage.md).


## Scene::OnEnd
```c++
void OnEnd(Theater::Play p) override
```
Is called once after [Transition](#scenetransitionto) was requested.
You should use this function to Remove Actors and free Ressources that where added during [OnStart](#sceneonstart) and [OnUpdate](#sceneonupdate).


## Scene::OnUpdate
```c++
void OnUpdate(Theater::Play p) override
```
This method is called each cycle.
Should the Stages playback be paused (via [Pause](./stage.md#accessable-functions)). This is the place, where you can `UnPause` the stage again.

> [!CAUTION]  
> If you call any of Raylib's Draw methods in here, they won't have any effect


## Scene::OnStageDraw
```c++
void OnStageDraw(Theater::Play p) override
```
This function is called each cycle, after all [visible Actors](./components.md#visible---component) have been drawn.

The width and height of drawing area are defined by ` p.stageWidth ` and ` p.stageHeight `. The Content drawn here is scaled, together with the [Stage](./stage.md)


## Scene::OnWindowDraw
```c++
void OnWindowDraw(Theater::Play p) override
```
This is similar to [OnStageDraw](#sceneonstagedraw), you are drawing directly to the window instead. No so scaling, no limitation.
The point 0,0 is the top left cornor of the Window.
No Scaling is applied to things Drawn in side this function.

