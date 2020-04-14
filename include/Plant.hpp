#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "Range.hpp"

class Plant {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        direction->serialize(ar, version);
        animation->serialize(ar, version);
        range->serialize(ar, version);
    }
private:
    Position* position; // Plant position
    Direction* direction; // Direction the plant is facing
    Animation* animation; // type of animation for the plant
    Range* range; // Represents how far plant can reach to attack
};