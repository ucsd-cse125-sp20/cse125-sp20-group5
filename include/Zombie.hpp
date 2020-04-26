#ifndef _ZOMBIE_H
#define _ZOMBIE_H

#include "GameObject.hpp"

class Zombie : public GameObject {
public:
    Zombie(Position* position, Direction* direction, Animation* animation, 
        unsigned int objectId) : GameObject(position, direction, animation, objectId){
        
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        GameObject::serialize(ar, version);
    }

    ~Zombie() {

    }
};

#endif
