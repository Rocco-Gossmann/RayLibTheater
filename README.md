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

(Also, so I can dust of my old C++ Books and maybe produce something that 
is reusable for other projects for once.)

# How to use this in your own Project.
1.) this is depended on **RayLib** (https://www.raylib.com/) 
So make sure, you follow their Setup-Guide here first [RayLib - Build and Installation](https://github.com/raysan5/raylib?tab=readme-ov-file#build-and-installation)

2.) Just copy  the [RayTheater.hpp](./src/lib/RayTheater.hpp) into your own Project.

2a.) Copy any [Additions](#additions), you want to have into the same folder

That is it. No further steps needed.
The Setup-Process is pretty much inspired by the 
**OneLoneCoder PixelGameEngine** (https://github.com/OneLoneCoder/olcPixelGameEngine)
(Always loved how simple it is to integrate into existing projects, so I wanted to mimik that)

# How to use it
[!todo]

# Advanced Techniques (Pre-Compiler Magic)
- [Custom-Attributes](./docs/custom_attributes.md)

# Additions
### RayTheaterUI.hpp
Contains predefined Actor-Classes, that can play UI-Components like Buttons, labels, etc.  

- Adding Buttons (TODO: Document this)

- Defining Default Themes (TODO: Document this)
