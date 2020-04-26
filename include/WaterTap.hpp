#ifndef _WATERTAP_H
#define _WATERTAP_H

#include "GameObject.hpp"

class WaterTap : public GameObject {
public:
    WaterTap(Position* position, Direction* direction, Animation * animation, 
        unsigned int objectId) : GameObject(position, direction, animation, objectId) {
        
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        GameObject::serialize(ar, version);
    }

    ~WaterTap() {
      
    }

};

#endif
