#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"

class Zombie {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        direction->serialize(ar, version);
        animation->serialize(ar, version);
    }
private:
    Position* position; // Zombie's position
    Direction* direction; // Direction the zombie is facing
    Animation* animation; // type of animation for the zombie
};