#ifndef _POSITION_H
#define _POSITION_H

class Position {
public:
    Position() : x(0), y(0), z(0) {}
    Position(float x, float y, float z): x(x), y(y), z(z) {}
    Position(const Position& that) {
        this->x = that.x;
        this->y = that.y;
        this->z = that.z;
    }
    Position(const Position* that) {
        this->x = that->x;
        this->y = that->y;
        this->z = that->z;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & x;
        ar & y;
        ar & z;
    }

    void setPosition(const Position& that) {
        this->x = that.x;
        this->y = that.y;
        this->z = that.z;
    }

    float x, y, z;
};

#endif
