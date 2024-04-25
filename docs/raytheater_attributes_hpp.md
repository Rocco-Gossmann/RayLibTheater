---
id: raytheater_attributes_hpp
aliases: []
tags: []
---

# RayTheaterAttributes.hpp
This file, contains info, for the Engine, that is implemented during Compile-Time.

# Where to put it
The File can be anywere in the Include-Path your Compiler can has access to.

So if you put the File into your `./src` folder, you must add a Compiler-Flag `-I./src`

# File Structure

### Header
you should add the following 3 Lines to the top of the File.
```cpp
#ifndef STAGE_ATTRIBUTE
#define STAGE_ATTRIBUTE(name) /**/
#endif
```

### Body
After the [Header](#header), you can start defining your custom Attributes.
```cpp
STAGE_ATTRIBUTE(MOUSEPOINTER)
STAGE_ATTRIBUTE(ENEMY)
STAGE_ATTRIBUTE(COLLECTABLE)
// ...
```

if everything went allright, your IDE should now see your new Attributes 
as part of the `Theater::Attributes` - Enum.  [ => Custom-Attributes ](./custom_attributes.md)

