#ifndef _TILE_H
#define _TILE_H

#include "Position.hpp"
#include "Direction.hpp"

class Tile {
public:
    Tile() : position(nullptr), direction(nullptr) {}

    Tile(Position* position, int tileType, Direction* direction)
        : tileType(tileType) {
        this->tileType = tileType;
        this->position = position;
        this->direction = direction;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & position;
        ar & tileType;
        ar & direction;
    }

    ~Tile() {
        delete position;
        delete direction;
    }

    Position* position; // Tile's position
    int tileType; // D/f types (e.g. normal, occupied by zombie, etc)

    Direction* direction; // For zombie tiles (e.g. which direction do they go)

    static constexpr const int TYPE_NORMAL = 0;
    static constexpr const int TYPE_ZOMBIE = 1;
    static constexpr const int TYPE_TILLED = 2;

    static constexpr const float TILE_SIZE = 1.0f;
    static constexpr const float TILE_PAD_X = TILE_SIZE / 2;
    static constexpr const float TILE_PAD_Y = TILE_SIZE / 2;
    static constexpr const float TILE_PAD_Z = TILE_SIZE / 2;
};

#endif
