#ifndef _WATERTAP_H
#define _WATERTAP_H

#include "Position.hpp"

class WaterTap {
public:
    WaterTap(Position* position, Direction* direction) {
        this->position = position;
        this->direction = direction;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        direction->serialize(ar, version);
    }

    ~WaterTap() {
      delete position;
      delete direction;
    }

    Position* position; // Water position
    Direction* direction; // andle of the tap
};

#endif
