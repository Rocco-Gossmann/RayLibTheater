#ifndef INTERFACE_H 
#define INTERFACE_H 1

#include <unordered_set>
#include <stdbool.h>
#include "play.h"

namespace Stage {

class Entity {
    friend class Stage;


public:
   
    Entity();

    enum Attributes {
        #define ENTITY_ATTRIBUTE(name) name,
        ENTITY_ATTRIBUTE(DEAD)
        #include "../EntityAttributes.hpp"
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

#endif // INTERFACE_H
