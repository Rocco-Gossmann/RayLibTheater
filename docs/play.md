# Play

`Play` is a Struct given to various eventhandlers, that interact with The RayTheater.

# Properties

```cpp
/** @brief Context provided to the Scene and each Actor on the Stage  */
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

  /** @brief Bitlist of mousebuttons just pressed this turn
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseDown;

  /** @brief Bitlist of mousebuttons held this turn
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseHeld;

  /** @brief Bitlist of mousebuttons not currently held or pressed
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseUp;

  /** @brief Bitlist of mousebuttons that where released last Frame
   * Mouse Button 1 =  (1 << 1) aka bit 2;
   * Mouse Button 2 =  (1 << 2) aka bit 4;
   * Mouse Button 3 =  (1 << 3) aka bit 8;
   * ...
   */
  unsigned char mouseReleased;

} Play
```
