---
id: play
aliases: []
tags: []
---

# Play

`Play` is a Struct given to various eventhandlers, that interact with The RayTheater.

# Properties
```cpp
typedef struct Play {

  /** @brief can be multiplied with an 'per second' value (like "pixels per
   * second") to achive framerate independed timing*/
  float deltaTime = 0.0f;

  /** @brief A reference to the stage, that alows interacting with it */
  Stage *stage = NULL;

  /** @brief a more detailed mousepoisition, that includes sub-pixel space
   * (For resized stages / windows) */
  Vector2 mouseLoc = {};

  /** @brief x-coordinate of the pixel, that the mouse is currently pointing at
   * at the stage */
  int mouseX = 0;

  /** @brief y-coordinate of the pixel, that the mouse is currently pointing at
   * at the stage */
  int mouseY = 0;

  /** @brief The available Stage Width in PX */
  int stageWidth = 0;

  /** @brief The available Stage Height in PX */
  int stageHeight = 0;

} Play;
```
