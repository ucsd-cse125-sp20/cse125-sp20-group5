#include "Position.hpp"

class Tool {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        ar & toolType;
        ar & heldBy;
    }
private:
    Position* position; // Tool position
    int toolType; // e.g. watering can, hoe, etc
    int heldBy; // Either held by players 1 - 4 or not held (0)
};