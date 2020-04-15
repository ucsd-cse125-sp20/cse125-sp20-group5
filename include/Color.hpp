#ifndef _COLOR_H
#define _COLOR_H

class Color {
public:
    Color(float r, float g, float b): r(r), g(g), b(b) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & r;
        ar & g;
        ar & b;
    }

private:
    float r, g, b; // RGB values?
};

#endif
