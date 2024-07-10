#ifndef RAYTHEATER_BUILDER_H
#define RAYTHEATER_BUILDER_H

#include "./Stage.hpp"
#include <raylib.h>
#include <type_traits>

namespace Theater {

/**
 * @class Builder
 * @brief meant to provide an easy way to setup a Stage.
 *
 * BM: Class
 */
class Builder {
private:
  Stage m_stage;

public:
  /**
   * @brief
   *
   * @param width  - the stages with in pixels
   * @param height - the stages height in pixels
   * @param scale  - multiplier by which the window will be scaled (e.g. 2.0 =
   * initially each pixel of the stage, takes 2 pixels in the Window)
   */
  Builder(int width, int height, float scale = 1.0) {
    // Setup the Window
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width * scale, height * scale, "Loading ...");

    if (!IsWindowReady())
      throw "window not ready";

    // Setup the Render-Area / Stage-Texture
    m_stage.m_stagerect.width = width;
    m_stage.m_stagerect.height = height;
    m_stage.m_stagetexture = LoadRenderTexture(width, height);

    if (!IsRenderTextureReady(m_stage.m_stagetexture)) {
      CloseWindow();
      throw "failed to create render texture";
    }

    BeginDrawing();
    EndDrawing();
    m_stage.onResize();
  }

  Builder *IntegerScale() {
    m_stage.m_integerScale = true;
    return this;
  }

  /**
   * @brief Sets the Title for the opened Window
   *
   * @param t - title
   * @return  itself, for easy chaining of setters
   */
  Builder *Title(const char *t) {
    SetWindowTitle(t);
    return this;
  }

  /**
   * @brief changes the color fo the border, that is shown, when
   * the window is scaled to a different aspect ratio
   *
   * @param c  - Raylib Color
   * @return  itself for easy chaining of setters
   */
  Builder *BorderColor(Color c) {
    m_stage.m_borderColor = c;
    return this;
  }

  /**
   * @brief sets the cleard background-color of the Stage
   *
   * @param c - Raylib Color
   * @return  itself for easy chainging of setters
   */
  Builder *BackgroundColor(Color c) {
    m_stage.m_backgroundColor = c;
    return this;
  }

  /**
   * @brief Opens the window and starts playing the given Scene
   * @param sc
   */
  template <typename T> void Play(T *sc) {
    static_assert(std::is_base_of<Stage::Scene, T>::value,
                  "Given Scene must be a child of class Stage::Scene");
    m_stage.ChangeScene((Stage::Scene *)sc);
    m_stage.play();
  }
};

} // namespace Theater

#endif
