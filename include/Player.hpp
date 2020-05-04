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

    void move() {
        float translateDistance = 0.0f;
        switch (moveState) {
        case MoveState::DOWN:
            translateDistance = checkRotation(Direction::DIRECTION_DOWN, false);
            position->z += translateDistance;
            break;
        case MoveState::LOWER_RIGHT:
            translateDistance = checkRotation(Direction::DIRECTION_LOWER_RIGHT, true);
            position->z += translateDistance;
            position->x += translateDistance;
            break;
        case MoveState::RIGHT:
            translateDistance = checkRotation(Direction::DIRECTION_RIGHT, false);
            position->x += translateDistance;
            break;
        case MoveState::UPPER_RIGHT:
            translateDistance = checkRotation(Direction::DIRECTION_UPPER_RIGHT, true);
            position->z -= translateDistance;
            position->x += translateDistance;
            break;
        case MoveState::UP:
            translateDistance = checkRotation(Direction::DIRECTION_UP, false);
            position->z -= translateDistance;
            break;
        case MoveState::UPPER_LEFT:
            translateDistance = checkRotation(Direction::DIRECTION_UPPER_LEFT, true);
            position->z -= translateDistance;
            position->x -= translateDistance;
            break;
        case MoveState::LEFT:
            translateDistance = checkRotation(Direction::DIRECTION_LEFT, false);
            position->x -= translateDistance;
            break;
        case MoveState::LOWER_LEFT:
            translateDistance = checkRotation(Direction::DIRECTION_LOWER_LEFT, true);
            position->z += translateDistance;
            position->x -= translateDistance;
            break;
        case MoveState::FREEZE:
            break;
        }
    }

    float checkRotation(float moveDirection, bool isDiagonal) {
        float translateDistance;
        if (direction->directionEquals(moveDirection)) {
            direction->angle = moveDirection;
            translateDistance = isDiagonal ? STEP_SIZE_DIAGONAL : STEP_SIZE;
        }
        else if (direction->clockwiseCloser(moveDirection)) {
            direction->angle += Direction::ROTATION_SPEED;
            direction->constrainAngle();
            translateDistance = isDiagonal ? IN_ROTATION_STEP_SIZE_DIAGONAL : IN_ROTATION_STEP_SIZE;
        }
        else {
            direction->angle -= Direction::ROTATION_SPEED;
            direction->constrainAngle();
            translateDistance = isDiagonal ? IN_ROTATION_STEP_SIZE_DIAGONAL : IN_ROTATION_STEP_SIZE;
        }
        return translateDistance;
    }

    Color* color;
    int playerId;
    bool holding;
    unsigned int heldObject;
    enum class MoveState { FREEZE, UP, DOWN, LEFT, RIGHT, UPPER_LEFT, UPPER_RIGHT, LOWER_LEFT, LOWER_RIGHT };
    MoveState moveState;

    static constexpr const float SQRT_2 = 1.41421356237309504880f;
    static constexpr const float STEP_SIZE = 0.15f;
    static constexpr const float STEP_SIZE_DIAGONAL = STEP_SIZE / SQRT_2;
    static constexpr const float IN_ROTATION_STEP_SIZE = 0.05f;
    static constexpr const float IN_ROTATION_STEP_SIZE_DIAGONAL = IN_ROTATION_STEP_SIZE / SQRT_2;
};

#endif
