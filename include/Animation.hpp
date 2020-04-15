#ifndef _ANIMATION_H
#define _ANIMATION_H

class Animation {
public:
    Animation(int animationType, int animationFrame)
        : animationType(animationType),
          animationFrame(animationFrame) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & animationType;
        ar & animationFrame;
    }
private:
    int animationType; // The current animation type (e.g. tool, player, etc)
    int animationFrame; // How far into the animation right now?
};

#endif
