
#ifndef ACTOR_DEBUG_H 
#define ACTOR_DEBUG_H 1

#include "../stage/theater.h"

namespace Actors {

class Debug : public Stage::Actor {

    public:
        bool OnTick(Stage::Play) override;

};

}

#endif // ACTOR_DEBUG_H
