#ifndef _WATERTAP_H
#define _WATERTAP_H

#include "Position.hpp"

class WaterTap {
public:
    WaterTap(Position* position) {
        this->position = position;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
    }

    ~WaterTap() {
      delete position;
    }

private:
    Position* position; // Water position
};

#endif
