#ifndef _ZOMBIE_H
#define _ZOMBIE_H

#include "GameObject.hpp"

class Zombie : public GameObject {
public:
    Zombie() : GameObject() {}
    Zombie(Position* position, Direction* direction, Animation* animation,
        unsigned int objectId) : GameObject(position, direction, animation, objectId) {
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
    }

    ~Zombie() {

    }

    static constexpr const float STEP_SIZE = 0.05f;
};

#endif
