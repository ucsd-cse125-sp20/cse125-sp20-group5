#ifndef _TOOL_H
#define _TOOL_H

#include "GameObject.hpp"

class Tool : public GameObject {
public:
    Tool() : GameObject(), toolType(0), heldBy(0) {}
    Tool(Position* position, Direction * direction, Animation * animation, 
        unsigned int objectId, float boundingBoxRadius, int toolType, int heldBy)
        : GameObject(position, direction, animation, objectId, boundingBoxRadius), toolType(toolType), heldBy(heldBy)  {
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

    int toolType; // e.g. watering can, hoe, etc
    unsigned int heldBy; // Player's objectId
};

#endif
