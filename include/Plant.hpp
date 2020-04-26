#ifndef _PLANT_H
#define _PLANT_H

#include "GameObject.hpp"
#include "TowerRange.hpp"

class Plant : GameObject {
public:
    Plant(Position* position, Direction* direction, Animation* animation, unsigned int objectId, 
        TowerRange* range) : GameObject(position, direction, animation, objectId) {

        this->range = range;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        GameObject::serialize(ar, version);
        range->serialize(ar, version);
    }

    ~Plant() {
        delete range;
    }

private:
    TowerRange* range; // Represents how far plant can reach to attack
};

#endif
