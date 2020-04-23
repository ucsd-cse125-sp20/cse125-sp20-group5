#ifndef _ZOMBIE_H
#define _ZOMBIE_H

#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"

class Zombie {
public:
    Zombie(Position* position, Direction* direction, Animation* animation) {
        this->position = position;
        this->direction = direction;
        this->animation = animation;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        direction->serialize(ar, version);
        animation->serialize(ar, version);
    }

    ~Zombie() {
        delete position;
        delete direction;
        delete animation;
    }

    Position* position; // Zombie's position
    Direction* direction; // Direction the zombie is facing
    Animation* animation; // type of animation for the zombie
};

#endif
