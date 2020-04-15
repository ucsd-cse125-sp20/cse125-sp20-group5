#ifndef _TILE_H
#define _TILE_H

#include "Position.hpp"
#include "Direction.hpp"

class Tile {
public:
    Tile(Position* position, int tileType, bool tilled, Direction* direction)
        : tileType(tileType), tilled(tilled) {

        this->position = position;
        this->direction = direction;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        ar & tileType;
        ar & tilled;
        direction->serialize(ar, version);
    }

    ~Tile() {
        delete position;
        delete direction;
    }

private:
    Position* position; // Tile's position
    int tileType; // D/f types (e.g. normal, occupied by zombie, etc)

    bool tilled; // For normal tiles

    Direction* direction; // For zombie tiles (e.g. which direction do they go)
};

#endif
