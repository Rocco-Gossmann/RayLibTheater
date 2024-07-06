#ifndef RAYTHEATER_TYPES_H
#define RAYTHEATER_TYPES_H

#include "./Play.h"
#include <functional>

namespace Theater {

typedef unsigned char byte;
typedef std::function<void()> TheaterDrawHandler;
typedef std::function<void(Stage *)> TheaterStageHandler;
typedef std::function<void(Stage *, Play)> TheaterHandler;
typedef unsigned int SceneUpdateRessource;
typedef unsigned int ActorRessource;

static const TheaterHandler NoHandler = [](Stage *, Play) {};
static const TheaterDrawHandler NoDrawHandler = []() {};
static const TheaterStageHandler NoStageHandler = [](Stage *) {};

} // namespace Theater
#endif