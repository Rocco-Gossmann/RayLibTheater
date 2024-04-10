
#ifndef STAGE_BUILDER_H 
#define STAGE_BUILDER_H 1

#include "stage.h"

namespace Stage {

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
    Builder (int width, int height, float scale = 1.0);

    Builder Title(const char*);

    Builder BorderColor(Color);
    Builder BackgroundColor(Color);

    void Play(Scene*);

private:
    Stage _stage;

};


}

#endif // STAGE_BUILDER_H
