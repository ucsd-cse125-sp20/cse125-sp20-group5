#ifndef _SEEDSHACK_H
#define _SEEDSHACK_H

#include "GameObject.hpp"

class SeedShack : public GameObject {
public:
    SeedShack() : GameObject() {}

    SeedShack(Position* position, Direction* direction, Animation* animation,
        unsigned int objectId, float boundingBoxRadius) : GameObject(position, direction, animation, objectId, boundingBoxRadius) {}

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
    }

    ~SeedShack() {
      
    }

};

#endif
