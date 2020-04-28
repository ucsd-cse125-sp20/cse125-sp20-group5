#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "TowerRange.hpp"
#include <boost/serialization/base_object.hpp>

class GameObject {
public:
    GameObject(): position(nullptr), direction(nullptr), animation(nullptr), objectId(0) {}

    GameObject(Position* position, Direction* direction, Animation* animation, 
        unsigned int objectId) : objectId(objectId){
        this->position = position;
        this->direction = direction;
        this->animation = animation;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & position;
        ar & direction;
        ar & animation;
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
