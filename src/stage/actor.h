#ifndef STAGE_ACTOR_H 
#define STAGE_ACTOR_H 1

#include <unordered_set>
#include <stdbool.h>
#include "play.h"

namespace Stage {

class Actor {
    friend class Stage;


public:

    enum Attributes {
        #define ENTITY_ATTRIBUTE(name) name,
        #include "../EntityAttributes.hpp"
        #include "internal_attribute_list.hpp"
    };

    virtual bool OnTick(Play);
    virtual void OnStageRemove(Play);

    bool hasAttribute(Attributes attr);

private:
    std::unordered_set<Attributes> _attributes;
    
    void addAttribute(Attributes a);
    void removeAttribute(Attributes a);

};


}

#endif // STAGE_ACTOR_H
