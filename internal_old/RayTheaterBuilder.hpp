#ifndef RAYTHEATER_BUILDER_H
#define RAYTHEATER_BUILDER_H

namespace Theater {

#include "./RayTheaterScene.hpp"
#include "./RayTheaterStage.hpp"
#include <raylib.h>

/**
 * @class Builder
 * @brief meant to provide an easy way to setup a Stage.
 *
 * BM: Class
 */
class Builder {
public:
  /**
   * @brief
   *
   * @param width  - the stages with in pixels
   * @param height - the stages height in pixels
   * @param scale  - multiplier by which the window will be scaled (e.g. 2.0 =
   * each pixel of the stage, takes 2 pixels in the Window)
   */
  Builder(int width, int height, float scale = 1.0)
      : _stage(Stage(width, height, scale)) {}

  Builder IntegerScale() {
    _stage._integerScale = true;
    return *this;
  }

  /**
   * @brief Sets the Title for the opened Window
   *
   * @param t - title
   * @return  itself, for easy chaining of setters
   */
  Builder Title(const char *t) {
    _stage._stageTitle = t;
    return *this;
  }

  /**
   * @brief changes the color fo the border, that is shown, when
   * the window is scaled to a different aspect ratio
   *
   * @param c  - Raylib Color
   * @return  itself for easy chaining of setters
   */
  Builder BorderColor(Color c) {
    _stage._borderColor = c;
    return *this;
  }

  /**
   * @brief sets the cleard background-color of the Stage
   *
   * @param c - Raylib Color
   * @return  itself for easy chainging of setters
   */
  Builder BackgroundColor(Color c) {
    _stage._backgroundColor = c;
    return *this;
  }

  /**
   * @brief Opens the window and starts playing the given Scene
   * @param sc
   */
  void Play(Scene *sc) { _stage.Play(sc); }

private:
  Stage _stage;
};

// BM: Implementation
//==============================================================================

} // namespace Theater

#endif
