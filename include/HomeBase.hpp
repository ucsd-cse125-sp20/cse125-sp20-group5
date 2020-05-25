#ifndef _HOMEBASE_H
#define _HOMEBASE_H

#include "GameObject.hpp"

class HomeBase : public GameObject {
public:
    HomeBase() : GameObject() {}
    HomeBase(Position* position, Direction* direction, Animation* animation, unsigned int objectId,
        float boundingBoxRadius) : GameObject(position, direction, animation, objectId, boundingBoxRadius) {
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<GameObject>(*this);
        ar& health;
        ar& maxHealth;
    }

    ~HomeBase() {

    }

    int health;
    int maxHealth;
};

#endif
