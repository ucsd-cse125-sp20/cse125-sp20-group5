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
    }

    ~Player() {
        delete color;
    }

    void move(float deltaTime) {
        float translateDistance = 0.0f;
        float speedX = 0.0f;
        float speedZ = 0.0f;
        switch (moveState) {
        case MoveState::DOWN:
            translateDistance = checkRotation(Direction::DIRECTION_DOWN, false, deltaTime);
            speedZ = 1.0f;
            break;
        case MoveState::LOWER_RIGHT:
            translateDistance = checkRotation(Direction::DIRECTION_LOWER_RIGHT, true, deltaTime);
            speedZ = 1.0f;
            speedX = 1.0f;
            break;
        case MoveState::RIGHT:
            translateDistance = checkRotation(Direction::DIRECTION_RIGHT, false, deltaTime);
            speedX = 1.0f;
            break;
        case MoveState::UPPER_RIGHT:
            translateDistance = checkRotation(Direction::DIRECTION_UPPER_RIGHT, true, deltaTime);
            speedZ = -1.0f;
            speedX = 1.0f;
            break;
        case MoveState::UP:
            translateDistance = checkRotation(Direction::DIRECTION_UP, false, deltaTime);
            speedZ = -1.0f;
            break;
        case MoveState::UPPER_LEFT:
            translateDistance = checkRotation(Direction::DIRECTION_UPPER_LEFT, true, deltaTime);
            speedZ = -1.0f;
            speedX = -1.0f;
            break;
        case MoveState::LEFT:
            translateDistance = checkRotation(Direction::DIRECTION_LEFT, false, deltaTime);
            speedX = -1.0f;
            break;
        case MoveState::LOWER_LEFT:
            translateDistance = checkRotation(Direction::DIRECTION_LOWER_LEFT, true, deltaTime);
            speedZ = 1.0f;
            speedX = -1.0f;
            break;
        case MoveState::FREEZE:
            break;
        }
        position->z += speedZ * translateDistance * deltaTime;
        position->x += speedX * translateDistance * deltaTime;
        currRow = position->z / Floor::TILE_SIZE;
        currCol = position->x / Floor::TILE_SIZE;
    }

    float checkRotation(float moveDirection, bool isDiagonal, float deltaTime) {
        float translateDistance;
        if (direction->directionEquals(moveDirection)) {
            direction->angle = moveDirection;
            translateDistance = isDiagonal ? SPEED_DIAGONAL : SPEED;
        }
        else if (direction->clockwiseCloser(moveDirection)) {
            direction->angle += Direction::ROTATION_SPEED * deltaTime;
            direction->constrainAngle();
            translateDistance = isDiagonal ? IN_ROTATION_SPEED_DIAGONAL : IN_ROTATION_SPEED;
        }
        else {
            direction->angle -= Direction::ROTATION_SPEED * deltaTime;
            direction->constrainAngle();
            translateDistance = isDiagonal ? IN_ROTATION_SPEED_DIAGONAL : IN_ROTATION_SPEED;
        }
        return translateDistance;
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

    static constexpr const float SQRT_2 = 1.41421356237309504880f;
    static constexpr const float SPEED = 4.8f;
    static constexpr const float SPEED_DIAGONAL = SPEED / SQRT_2;
    static constexpr const float IN_ROTATION_SPEED = 1.6f;
    static constexpr const float IN_ROTATION_SPEED_DIAGONAL = IN_ROTATION_SPEED / SQRT_2;
};

#endif
