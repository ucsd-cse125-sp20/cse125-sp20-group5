#ifndef _SEEDSHACK_H
#define _SEEDSHACK_H

#include "Position.hpp"

class SeedShack {
public:
    SeedShack() : position(nullptr) {}

    SeedShack(Position* position) {
      this->position = position;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & position;
    }

    ~SeedShack() {
      delete position;
    }

private:
    Position* position; // Seed position
};

#endif
