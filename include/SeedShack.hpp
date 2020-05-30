#ifndef _SEEDSHACK_H
#define _SEEDSHACK_H

#include "GameObject.hpp"
#include "Plant.hpp"

class SeedShack : public GameObject {
public:
    SeedShack() : GameObject(), seedType(Plant::PlantType::CORN) {}

    SeedShack(Position* position, Direction* direction, Animation* animation,
        unsigned int objectId, float boundingBoxRadius, Plant::PlantType seedType) : GameObject(position, direction, animation, objectId, boundingBoxRadius) {
        this->seedType = seedType;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & seedType;
    }

    ~SeedShack() {
      
    }

    Plant::PlantType seedType;
};

#endif
