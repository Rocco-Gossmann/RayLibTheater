#include "actor.h"


namespace Stage {

bool Actor::OnTick(Play) { return false; }
void Actor::OnStageRemove(Play) {}

void Actor::addAttribute(Attributes a) {
    _attributes.insert(a);
}

void Actor::removeAttribute(Attributes a) {
    auto attr = this->_attributes.find(a);  

    if(attr != _attributes.end()) {
        _attributes.erase(a);
    }
}

bool Actor::hasAttribute(Attributes a) {
    return _attributes.find(a) != _attributes.end();
}

}
