#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "TowerRange.hpp"
#include <cmath>
#include <boost/serialization/base_object.hpp>

class GameObject {
public:
    GameObject(): position(nullptr), direction(nullptr), animation(nullptr), objectId(0), boundingBoxRadius(0.0) {}

    GameObject(Position* position, Direction* direction, Animation* animation, 
        unsigned int objectId, float radius) : objectId(objectId), boundingBoxRadius(radius) {
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
        ar & boundingBoxRadius;
    }

    ~GameObject() {
        delete position;
        delete direction;
        delete animation;
    }

    float distanceTo(GameObject* that) {
        return std::sqrt((position->x - that->position->x) * (position->x - that->position->x) + (position->z - that->position->z) * (position->z - that->position->z));
    }

    bool collideWith(GameObject* that) {
        float distance = this->distanceTo(that);
        return distance <= boundingBoxRadius + that->boundingBoxRadius;
    }

    Position* position; // Plant position
    Direction* direction; // Direction the  facing
    Animation* animation; // type of animation
    float boundingBoxRadius; // radius of the Circle Bounding Box
    unsigned int objectId;
};

#endif
