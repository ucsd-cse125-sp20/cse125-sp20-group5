#ifndef _PLAYER_H
#define _PLAYER_H

#include <cmath>
#include "GameObject.hpp"
#include "Color.hpp"
#include "Floor.hpp"

class Player : public GameObject {
public:
    Player() : GameObject(), color(nullptr), playerId(0), holding(false), heldObject(0) {}

    Player(Position* position, Direction* direction,
           Animation* animation, unsigned int objectId, float boundingBoxRadius,
           Color* color, int playerId)
            : GameObject(position, direction, animation, objectId, boundingBoxRadius), 
              playerId(playerId) {
        this->color = color;
        currRow = position->z / Floor::TILE_SIZE;
        currCol = position->x / Floor::TILE_SIZE;
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
        ar & highlightObjectId;
    }

    ~Player() {
        delete color;
    }

    Color* color;
    int playerId;

    int health;
    int maxHealth;

    // Tool
    bool holding;
    unsigned int heldObject;

    // Movement
    enum class MoveState { FREEZE, UP, DOWN, LEFT, RIGHT, UPPER_LEFT, UPPER_RIGHT, LOWER_LEFT, LOWER_RIGHT };
    MoveState moveState;

    // Board locaton index
    int currRow;
    int currCol;

    // Should player perform action/interact in this tick
    bool shouldPerformAction;
    bool shouldInteract;
    
    // Highlighting object
    unsigned int highlightObjectId;
};

#endif
