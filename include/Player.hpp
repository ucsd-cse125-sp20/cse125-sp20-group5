#ifndef _PLAYER_H
#define _PLAYER_H

#include "GameObject.hpp"
#include "Color.hpp"

class Player : public GameObject {
public:
    Player() : GameObject(), color(nullptr), playerId(0), holding(false), heldObject(0) {}

    Player(Position* position, Direction* direction,
           Animation* animation, unsigned int objectId, float boundingBoxRadius,
           Color* color, int playerId)
            : GameObject(position, direction, animation, objectId, boundingBoxRadius), 
              playerId(playerId) {

        this->color = color;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & color;
        ar & playerId;
        ar & holding;
        ar & heldObject;
    }

    ~Player() {
        delete color;
    }

    Color* color;
    int playerId;
    bool holding;
    unsigned int heldObject;
};

#endif
