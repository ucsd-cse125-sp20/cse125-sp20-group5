#ifndef _TOOL_H
#define _TOOL_H

#include "GameObject.hpp"

class Tool : public GameObject {
public:
    Tool(Position* position, Direction * direction, Animation * animation, 
        unsigned int objectId, int toolType, int heldBy)
        : GameObject(position, direction, animation, objectId), toolType(toolType), heldBy(heldBy)  {
        this->position = position;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        GameObject::serialize(ar, version);
        ar & toolType;
        ar & heldBy;
    }

    ~Tool() {
        //delete position;
    }

private:
    //Position* position; // Tool position
    int toolType; // e.g. watering can, hoe, etc
    int heldBy; // Either held by players 1 - 4 or not held (0)
};

#endif
