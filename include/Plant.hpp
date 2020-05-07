#ifndef _PLANT_H
#define _PLANT_H

#include "GameObject.hpp"
#include "TowerRange.hpp"

class Plant : public GameObject {
public:
    Plant(): GameObject(), range(nullptr) {}

    Plant(Position* position, Direction* direction, Animation* animation, unsigned int objectId, float boundingBoxRadius,
        TowerRange* range) : GameObject(position, direction, animation, objectId, boundingBoxRadius) {
        this->range = range;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & range;
    }

    ~Plant() {
        delete range;
    }

private:
    TowerRange* range; // Represents how far plant can reach to attack
};

#endif
