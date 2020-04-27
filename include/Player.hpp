#ifndef _PLAYER_H
#define _PLAYER_H

#include "GameObject.hpp"
#include "Color.hpp"

class Player : public GameObject {
public:
    Player(Position* position, Direction* direction,
           Animation* animation, unsigned int objectId, Color* color, int playerId)
            : GameObject(position, direction, animation, objectId), playerId(playerId) {

        this->color = color;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        GameObject::serialize(ar, version);
        color->serialize(ar, version);
        ar & playerId;
        ar & holding;
        ar & heldObject;
    }

    ~Player() {
        delete color;
    }

    void updatePosition() {
        position->update(
            position->getX() + 0.1,
            position->getY(),
            position->getZ() + 0.1);
    }

   // Position* position; // Player position
    //Direction* direction; // Direction player is facing
    //Animation* animation; // type of animation for player
    Color* color;
    int playerId;
    bool holding;
    unsigned int heldObject;
};

#endif
