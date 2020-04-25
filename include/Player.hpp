#ifndef _PLAYER_H
#define _PLAYER_H

#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "Color.hpp"

class Player {
public:
    Player() : position(nullptr), direction(nullptr), animation(nullptr), color(nullptr) {}

    Player(Position* position, Direction* direction,
           Animation* animation, Color* color, int playerId)
            : playerId(playerId) {

        this->position = position;
        this->direction = direction;
        this->animation = animation;
        this->color = color;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & position;
        ar & direction;
        ar & animation;
        ar & color;
        ar & playerId;
    }

    ~Player() {
        delete position;
        delete direction;
        delete animation;
        delete color;
    }

    Position* position; // Player position
    Direction* direction; // Direction player is facing
    Animation* animation; // type of animation for player
    Color* color;
    int playerId;
};

#endif
