#ifndef RAYTHEATER_TYPES_H
#define RAYTHEATER_TYPES_H

#include "./internal/ActorAttributes.h"
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace Theater {

typedef unsigned char byte;
typedef unsigned int SceneUpdateRessource;
typedef unsigned int ActorRessource;
typedef std::unordered_set<ActorRessource> ActorRessourceList;
typedef std::unordered_map<ATTRIBUTES, ActorRessourceList> AttributeActorList;
typedef std::unordered_set<ATTRIBUTES> AttributeList;

} // namespace Theater
#endif
