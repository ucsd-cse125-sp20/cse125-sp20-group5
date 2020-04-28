#ifndef _SEEDSHACK_H
#define _SEEDSHACK_H

#include "GameObject.hpp"

class SeedShack : public GameObject {
public:
    SeedShack(Position* position, Direction * direction, Animation* animation, 
        unsigned int objectId) : GameObject(position, direction, animation, objectId) {
      
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        GameObject::serialize(ar, version);
    }

    ~SeedShack() {
      
    }

};

#endif
