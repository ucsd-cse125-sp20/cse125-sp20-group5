#ifndef _WATERTAP_H
#define _WATERTAP_H

#include "GameObject.hpp"

class WaterTap : public GameObject {
public:
    WaterTap() : GameObject() {}
    WaterTap(Position* position, Direction* direction, Animation* animation,
        unsigned int objectId, float boundingBoxRadius) : GameObject(position, direction, animation, objectId, boundingBoxRadius) {}

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
    }

    ~WaterTap() {
      
    }
};

#endif
