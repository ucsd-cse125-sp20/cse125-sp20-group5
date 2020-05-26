#ifndef _ZOMBIE_H
#define _ZOMBIE_H

#include "GameObject.hpp"
#include "Tile.hpp"
#include "ServerParams.h"

class Zombie : public GameObject {
public:
    enum class ZombieType;

    Zombie() : GameObject() {}
    Zombie(Position* position, Direction* direction, Animation* animation, unsigned int objectId,
        float boundingBoxRadius, Zombie::ZombieType zombieType)
      : GameObject(position, direction, animation, objectId, boundingBoxRadius),
        zombieType(zombieType) {
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & health;
        ar & maxHealth;
        ar & zombieType;
    }

    ~Zombie() {

    }

    static Zombie* buildZombie(ServerParams& config, ZombieType type, Tile* zombieBaseTile) {
        Zombie* zombie = new Zombie();
        zombie->position = new Position(zombieBaseTile->getCenterPosition());
        zombie->direction = new Direction(zombieBaseTile->direction->angle);
        zombie->animation = new Animation(0, 0);
        zombie->zombieType = type;
        switch (type) {
        case ZombieType::RABBIT:
            zombie->moveSpeed = config.zombieRabbitMoveSpeed;
            zombie->boundingBoxRadius = config.zombieRabbitRadius;
            break;
        case ZombieType::PIG:
            zombie->moveSpeed = config.zombiePigMoveSpeed;
            zombie->boundingBoxRadius = config.zombiePigRadius;
            break;
        }
        return zombie;
    }


    static enum class ZombieType { RABBIT = 0, PIG = 1 };

    int health;
    int maxHealth;
    float moveSpeed;
    ZombieType zombieType;

    // Animation Type
    enum ZombieAnimation { MOVE, DIE };
};

#endif
