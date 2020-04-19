#ifndef _DIRECTION_H
#define _DIRECTION_H

class Direction {
public:
    Direction(float angle): angle(angle) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & angle;
    }

private:
    float angle; // The angle that the plant is facing??
};

#endif
