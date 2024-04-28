---
id: button
aliases: []
tags: []
---

# RayTheater - UI - Button

Buttons are Elements, that can be interacted with, via a Mouse, Touchscreen or other Pointer devices.

## Defining a Button

First, define the Button itself.
```cpp
int buttonId = 2;  // Passed back to functions handelding the Button Events

float posX = 8;    // Coordinates of the button on the stage in pixels 
float posY = 8;    

float width = 64;   // Width and Height of the Button in Pixels
float heigth = 24; 

auto myButton = Theater::UI::Button(buttonId, posX, posY, width, height);
```
By calling the Buttons `Label(string)` Setter, you you can change, what is written on it.

```cpp
myButton.Label("my Btn");
```


Next add and remove it to/from the Stage, as you would with any other Actor.
```cpp

//...
public:
    void OnLoad(Play p) {
        p.stage->AddActor(&myButton);
        // ...
    }

    Scene* OnUnload(Play p) {
        p.stage->RemoveActor(&myButton);
        // ...
    }
// ...
```

## Handeling Events / Clicks

All events are handled through a Lamda function, that has the following Format.
```cpp
void(int, ButtonEvent, Button *)
```

You register the Lamda function via various `On...` methods on the Button Instances.

### Event-Types:
| Event   | Method      | EventType     | Description                                                                 |
|---------|-------------|---------------|-----------------------------------------------------------------------------|
| Hover   | `OnHover`   | `BTN_HOVER`   | Called as soon as the Mouse is over the Button, but not pressed             |
| Press   | `OnPress`   | `BTN_PRESS`   | Called during the Cycle, the Button is first pressed                        |
| Hold    | `OnHold`    | `BTN_HOLD`    | Called during each extra Cycle that the Mouse is held down                  |
| Release | `OnRelease` | `BTN_RELEASE` | Called once a Button is no longer held down or pressed                      |
| Out     | `OnOut`     | `BTN_OUT`     | Called when the Mouse-Cursor leaves the button while it was pressed or held |

For ease of use, there is also the `OnEvent` Method, that registers the given 
Lamda function for all Events of a Button.

To Remove an Event-Handler simple call it's method with a `NULL`/`nullptr`/`0x0` value.

All Setters can be chained.

example: 
```cpp
  auto OnButton = [](int id, Button::ButtonEvent eventType, Button *btn) {
    switch(eventType) {

        case BTN_PRESS  : cout << "Btn (id: " << id << ") PRESS  " << endl; break;
        case BTN_RELEASE: cout << "Btn (id: " << id << ") RELEASE" << endl; break;
        case BTN_OUT    : cout << "Btn (id: " << id << ") OUT    " << endl; break;

    }
  };

  auto OnButtonHover = [](int id, Button::ButtonEvent eventType, Button *btn) {
      cout << " Button hover " << endl;
  };

  myButton.OnEvent(&OnButton)   // Register the above function 
                                // for all Events of the Button
    ->OnHover(&OnButtonHover)   // but handle hover by another function 
    ->OnHold(NULL)              // And disable handling for Hold Events
  ;
```
## Styling

Buttons come with a:
```cpp
Button* Style(Theater::UI::Button::UIStyle*) 
```
Setter Method.

The Buttons appearance can be modified via UIStyle


