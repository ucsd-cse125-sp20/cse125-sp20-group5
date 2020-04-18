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

    void update(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float getX() {
        return x;
    }

    float getY() {
        return y;
    }
    float getZ() {
        return z;
    }

private:
    float x, y, z;
};

#endif
