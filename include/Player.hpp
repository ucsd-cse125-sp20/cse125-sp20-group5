#include "Position.hpp"
#include "Direction.hpp"
#include "Animation.hpp"
#include "Color.hpp"

class Player {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        position->serialize(ar, version);
        direction->serialize(ar, version);
        animation->serialize(ar, version);
        color->serialize(ar, version);
        ar & playerId;
    }
private:
    Position* position; // Player position
    Direction* direction; // Direction player is facing
    Animation* animation; // type of animation for player
    Color* color;
    int playerId;
};