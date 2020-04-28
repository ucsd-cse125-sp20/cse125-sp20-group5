#ifndef _TOOL_H
#define _TOOL_H

#include "GameObject.hpp"

class Tool : public GameObject {
public:
    Tool(Position* position, Direction * direction, Animation * animation, 
        unsigned int objectId, int toolType, bool held)
        : GameObject(position, direction, animation, objectId), toolType(toolType), held(held)  {
        this->position = position;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        GameObject::serialize(ar, version);
        ar & toolType;
        ar & held;
    }

    ~Tool() {
        //delete position;
    }

    int toolType; // e.g. watering can, hoe, etc
    bool held; // Either held by players 1 - 4 or not held (0)
};

#endif
