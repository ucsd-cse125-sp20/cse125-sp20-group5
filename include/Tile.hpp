#include "Position.hpp"
#include "Direction.hpp"

class Tile {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        ar & tileType;
        ar & tilled;
        direction->serialize(ar, version);
    }
private:
    Position* position; // Tile's position
    int tileType; // D/f types (e.g. normal, occupied by zombie, etc)

    bool tilled; // For normal tiles

    Direction* direction; // For zombie tiles (e.g. which direction do they go)
};