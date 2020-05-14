#ifndef _DIRECTION_H
#define _DIRECTION_H
#include <cmath>
class Direction {
public:
    Direction() : angle(0) {}
    Direction(float angle): angle(angle) {}
    Direction(const Direction* that) {
        this->angle = that->angle;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & angle;
    }

    bool clockwiseCloser(float direction) {
        float diff = angle - direction;
        if (diff < 0) {
            diff += 2 * PI;
        }

        return diff >= PI;
    }

    bool directionEquals(float direction) {
        return std::abs(angle - direction) < EPSILON
            || std::abs(angle - direction - TWO_PI) < EPSILON;
    }

    void constrainAngle() {
        angle = fmod(angle, TWO_PI);
        if (angle < 0)
            angle += TWO_PI;
    }

    float angle; // The angle that the plant is facing??

    static constexpr const float EPSILON = 0.001f;
    static constexpr const float PI = 3.14159265358979323846f;
    static constexpr const float TWO_PI = 2 * PI;
    static constexpr const float FOUR_PI = 4 * PI;
    static constexpr const float PI_2 = 1.57079632679489661923f;
    static constexpr const float PI_4 = 0.785398163397448309616f;

    static constexpr const float DIRECTION_DOWN = 0.0f;
    static constexpr const float DIRECTION_LOWER_RIGHT = PI_4;
    static constexpr const float DIRECTION_RIGHT = PI_2;
    static constexpr const float DIRECTION_UPPER_RIGHT = 3 * PI_4;
    static constexpr const float DIRECTION_UP = PI;
    static constexpr const float DIRECTION_UPPER_LEFT = 5 * PI_4;
    static constexpr const float DIRECTION_LEFT = 3 * PI_2;
    static constexpr const float DIRECTION_LOWER_LEFT = 7 * PI_4;

    static constexpr const float ROTATION_SPEED = 4 * PI;
};

#endif
