#include "entity.h"


namespace Stage {

Entity::Entity() {}

void Entity::addAttribute(Attributes a) {
    _attributes.insert(a);
}

void Entity::removeAttribute(Attributes a) {
    auto attr = this->_attributes.find(a);  

    if(attr != _attributes.end()) {
        _attributes.erase(a);
    }
}

bool Entity::hasAttribute(Attributes a) {
    return _attributes.find(a) != _attributes.end();
}

}
