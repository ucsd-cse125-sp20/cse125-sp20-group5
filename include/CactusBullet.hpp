#ifndef _CACTUS_BULLET_H
#define _CACTUS_BULLET_H

#include "GameObject.hpp"

class CactusBullet : public GameObject {
public:
    CactusBullet() : GameObject() {}
    CactusBullet(Position* position, Direction* direction, Animation* animation,
        unsigned int objectId, float boundingBoxRadius) : GameObject(position, direction, animation, objectId, boundingBoxRadius) {}

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & attackPower;
    }

    ~CactusBullet() {

    }

    int attackPower;
};


#endif