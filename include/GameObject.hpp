#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "TowerRange.hpp"

class GameObject {
public:
    GameObject(Position* position, Direction* direction, Animation* animation, 
        int objectId) : objectId(objectId){
        this->position = position;
        this->direction = direction;
        this->animation = animation;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        direction->serialize(ar, version);
        animation->serialize(ar, version);
        ar & objectId;
    }

    ~GameObject() {
        delete position;
        delete direction;
        delete animation;
    }

    Position* position; // Plant position
    Direction* direction; // Direction the  facing
    Animation* animation; // type of animation
    unsigned int objectId;
};

#endif
