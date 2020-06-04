#ifndef _PLAYER_H
#define _PLAYER_H

#include <cmath>
#include "GameObject.hpp"
#include "Color.hpp"
#include "Floor.hpp"
#include <map>

class Player : public GameObject {
public:
    Player() : GameObject(), color(nullptr), playerId(0), holding(false), heldObject(0) {}

    Player(Position* position, Direction* direction,
           Animation* animation, unsigned int objectId, float boundingBoxRadius,
           Color* color, int playerId)
            : GameObject(position, direction, animation, objectId, boundingBoxRadius), 
              playerId(playerId) {
        this->color = color;
        this->currChat = NO_CHAT;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & color;
        ar & playerId;
        ar & health;
        ar & maxHealth;
        ar & isDead;
        ar & holding;
        ar & heldObject;
        ar & highlightObjectId;
        ar & highlightTileRow;
        ar & highlightTileCol;
        ar & invincibleTime;
        ar & currChat;
    }

    ~Player() {
        delete color;
    }

    Color* color;
    int playerId;

    int health;
    int maxHealth;
    bool isDead;

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
    unsigned int highlightObjectId; //  0 indicates not highlighting
    int highlightTileRow;           // -1 indicates not highlighting
    int highlightTileCol;           // -1 indicates not highlighting

    // Invincible time after hitting by zombie
    float invincibleTime;

    // Current chat blob type
    int currChat;
    const static std::string chatMessages[11];
    static constexpr int NO_CHAT = -1;

    // Animation Type
    enum PlayerAnimation { IDLE, MOVE, WATER, PLOUGH };
};

const std::string Player::chatMessages[11] = {
    "Water!", "Shovel!", "Pesticide!", "Fertilizer!", "Come on!!", // 0~4
    "Help!!!", "I need somebody", "Thanks!", "Good job!", "GG", // 5~9
    "Thank you for the quarter!"// secret
};

#endif
