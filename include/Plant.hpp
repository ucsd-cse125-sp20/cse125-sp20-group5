#ifndef _PLANT_H
#define _PLANT_H

#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "TowerRange.hpp"

class Plant {
public:
    Plant() : position(nullptr), direction(nullptr), animation(nullptr), range(nullptr) {}

    Plant(Position* position, Direction* direction,
          Animation* animation, TowerRange* range) {
        this->position = position;
        this->direction = direction;
        this->animation = animation;
        this->range = range;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & position;
        ar & direction;
        ar & animation;
        ar & range;
    }

    ~Plant() {
        delete position;
        delete direction;
        delete animation;
        delete range;
    }

private:
    Position* position; // Plant position
    Direction* direction; // Direction the plant is facing
    Animation* animation; // type of animation for the plant
    TowerRange* range; // Represents how far plant can reach to attack
};

#endif
