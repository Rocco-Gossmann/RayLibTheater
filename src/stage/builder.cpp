#include "builder.h"
#include "scene.h"

namespace Stage {

Builder::Builder(int width, int height, float scale)
: _stage(Stage(width, height, scale)) {}

Builder Builder::Title(const char* t) {
    _stage._stageTitle = t;
    return *this;
}

Builder Builder::BorderColor(Color c) {
    _stage._borderColor = c;
    return *this;
}

Builder Builder::BackgroundColor(Color c) {
    _stage._backgroundColor = c;
    return *this;
}

void Builder::Play(Scene* sc) {
    _stage.Play(sc);
}


}
