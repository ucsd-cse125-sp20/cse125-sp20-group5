#include "Position.hpp"

class WaterTap {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
    }
private:
    Position* position; // Water position
};