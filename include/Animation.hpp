#ifndef _ANIMATION_H
#define _ANIMATION_H

class Animation {
public:
    Animation() : animationType(0), animationFrame(0) {}
    Animation(int animationType, float animationFrame)
        : animationType(animationType),
          animationFrame(animationFrame) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & animationType;
        ar & animationFrame;
    }

    int animationType; // The current animation type (e.g. tool, player, etc)
    float animationFrame; // How far into the animation right now?
};

#endif
