#ifndef _POSITION_H
#define _POSITION_H

class Position {
public:
    Position(float x, float y, float z): x(x), y(y), z(z) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & x;
        ar & y;
        ar & z;
    }

private:
    float x, y, z;
};

#endif
