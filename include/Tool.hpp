#ifndef _TOOL_H
#define _TOOL_H

#include "GameObject.hpp"


class Tool : public GameObject {
public:
    enum class ToolType;

    Tool() : GameObject(), toolType(ToolType::WATER_CAN), heldBy(0), held(false) {}
    Tool(Position* position, Direction * direction, Animation * animation, 
        unsigned int objectId, float boundingBoxRadius, ToolType toolType, int heldBy, bool held)
        : GameObject(position, direction, animation, objectId, boundingBoxRadius), toolType(toolType), heldBy(heldBy), held(held)  {
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & toolType;
        ar & seedType;
        ar & heldBy;
        ar & held;
        ar & remainingWater;
        ar & capacity;
        ar & fertilizerCurrTime;
        ar & fertilizerCooldownTime;
    }

    ~Tool() {
    }

    static enum class ToolType { WATER_CAN = 0, PLOW = 1, SEED = 2, PESTICIDE = 3, FERTILIZER = 4 };
    ToolType toolType; // e.g. watering can, hoe, etc
    Plant::PlantType seedType;      // only used when toolType is SEED
    
    float remainingWater; // only used when toolType is WATER_CAN
    float capacity;       // only used when toolTye is WATER_CAN

    float fertilizerCurrTime;     // only used when toolType is FERTILIZER
    float fertilizerCooldownTime; // only used when toolType is FERTILIZER

    unsigned int heldBy; // Player's objectId
    bool held;
};

#endif
