#ifndef _TOOL_H
#define _TOOL_H

#include "GameObject.hpp"

class Tool : public GameObject {
public:
    Tool() : GameObject(), toolType(0), heldBy(0) {}
    Tool(Position* position, Direction * direction, Animation * animation, 
        unsigned int objectId, int toolType, int heldBy)
        : GameObject(position, direction, animation, objectId), toolType(toolType), heldBy(heldBy)  {
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & toolType;
        ar & heldBy;
    }

    ~Tool() {
    }

private:
    int toolType; // e.g. watering can, hoe, etc
    int heldBy; // Either held by players 1 - 4 or not held (0)
};

#endif
