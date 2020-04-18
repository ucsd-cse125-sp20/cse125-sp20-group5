#ifndef _PLAYER_H
#define _PLAYER_H

#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "Color.hpp"

class Player {
public:
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
        position->serialize(ar, version);
        direction->serialize(ar, version);
        animation->serialize(ar, version);
        color->serialize(ar, version);
        ar & playerId;
    }

    ~Player() {
        delete position;
        delete direction;
        delete animation;
        delete color;
    }

    void updatePosition() {
        position->update(
            position->getX() + 1,
            position->getY() + 1,
            position->getZ() + 1);
    }

private:
    Position* position; // Player position
    Direction* direction; // Direction player is facing
    Animation* animation; // type of animation for player
    Color* color;
    int playerId;
};

#endif
