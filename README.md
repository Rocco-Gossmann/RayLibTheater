# RayTheater

A Wrapper around RayLib, written in C++.
Providing a Framework to take off some unfun burdens of Software development.
Like:

- Automatic handeling of Window-Scaling.
- Switching between different Application-States / Views.
- Handeling Entities / Entity Components

---

# Why does this exists?

Just because. :-)

(Also, so I can dust of my old C++ Books and maybe produce something that is reusable for other projects for once.)

# How to add this to your own Project.

1.) this is depended on **RayLib** (https://www.raylib.com/)
So make sure, you follow their Setup-Guide here first [RayLib - Build and Installation](https://github.com/raysan5/raylib?tab=readme-ov-file#build-and-installation)

2.) Just copy the [RayTheater.hpp](./src/lib/RayTheater.hpp) into your own Project.

2a.) Copy any [Additions](#additions), you want to have into the same folder

That is it. No further steps needed.
The Setup-Process is pretty much inspired by the
**OneLoneCoder PixelGameEngine** (https://github.com/OneLoneCoder/olcPixelGameEngine)
(Always loved how simple it is to integrate into existing projects, so I wanted to mimik that)

# How to use it in Code.

1.) Include the File

```cpp
#include "RayTheater.hpp"

int main() {
    // ...
```

2.) define a Scene.

```cpp
#include "RayTheater.hpp"
// Since RayTheater is just a RayLib addon, you can
// Raylib needs to be installed as well
#include <raylib.h>

class MyScene : public Theater::Scene {
public:
    void OnStageDraw(Theater::Play p) {
        // Inside any the "...Draw" method of RayTheater,
        // you can use RayLibs native Draw function
        DrawText("Hello World", 8, 8, 20, GREEN);
    }

    void OnStart(Theater::Play p) { /* Optional override */ }
    void OnUpdate(Theater::Play p) { /* Optional override */ }
    void OnWindowDraw(Theater::Play p) { /* Optional override */ }
    void OnEnd(Theater::Play p) { /* Optional override */ }
};


int main() {
    // ...
```

3.) Build the Stage and Play the Scene in your `main` - function

```cpp

#include "RayTheater.hpp"
#include <raylib.h>

class MyScene : public Theater::Scene {
//...
};

int main() {
    MyScene ms;

    Theater::Builder(
        480, // Pixel width of the stage
        320, // Pixel height of the stage
        2    // Initial Window Scale (2x)
    )
        .Title("RayLib - Window 🎉") // Giving the Window
        .Play(&ms);

    return 0;
}
```

Here is the full version of that code. You can just copy and paste it into your main.cpp, to check if your project setup works.

```cpp

#include "RayTheater.hpp"
#include <raylib.h>

class MyScene : public Theater::Scene {
public:
    void OnStageDraw(Theater::Play p) {
        DrawText("Hello World", 8, 8, 20, GREEN);
    }
};

int main() {
    MyScene ms;

    Theater::Builder(480, 320,  2)
        .Title("RayLib - Window 🎉")
        .Play(&ms);

    return 0;
}
```

# Documentation

- Theater::Builder [!todo]

- Theater::Scene [!todo]

- [Theater::Play](./docs/play.md)
  Is proveded to The Scene and All Actors while the Scene is running.

  - [Theater::Stage](./docs/stage.md)
    Provided through Theater::Play and is used to Add, Read, Remove or Modify Actors on the Stage.

# Advanced Techniques (Pre-Compiler Magic)

- [Custom-Attributes](./docs/custom_attributes.md)

# Additions

### RayTheaterUI.hpp

Contains predefined Actor-Classes, that can play UI-Components like Buttons, labels, etc.
[goto Documentation](./docs/additions/ui.md)

- Adding Buttons (TODO: Document this)

- Defining Default Themes (TODO: Document this)
