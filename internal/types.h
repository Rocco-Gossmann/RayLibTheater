#ifndef RAYTHEATER_TYPES_H
#define RAYTHEATER_TYPES_H

#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "./ActorAttributes.h"

namespace Theater {

class Timer;

typedef unsigned char byte;
typedef unsigned int SceneUpdateRessource;
typedef unsigned int ActorRessource;
typedef std::unordered_set<ActorRessource> ActorRessourceList;
typedef std::unordered_map<ATTRIBUTES, ActorRessourceList> AttributeActorList;
typedef std::unordered_set<ATTRIBUTES> AttributeList;

typedef unsigned int TimerRessource;
typedef std::unordered_map<TimerRessource, Timer *> TimerList;

} // namespace Theater
#endif
