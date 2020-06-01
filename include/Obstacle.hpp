#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

#include "GameObject.hpp"

class Obstacle : public GameObject {
public:
    enum class ObstacleType;
    Obstacle() : GameObject() {}
    Obstacle(Position* position, Direction* direction, Animation* animation,
        unsigned int objectId, float boundingBoxRadius, ObstacleType obstacleType)
      : GameObject(position, direction, animation, objectId, boundingBoxRadius),
        obstacleType(obstacleType){}

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<GameObject>(*this);
        ar & obstacleType;
    }

    ~Obstacle() {

    }

    static enum class ObstacleType { TREE = 0, STONE = 1 };
    ObstacleType obstacleType;
};

#endif
