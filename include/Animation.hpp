#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <chrono>

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
    std::chrono::system_clock::time_point animStartTime;

    /* methods */

    void update() { // TODO should be called GameObject's update, which should be called in server code
        std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - animStartTime;
        animationFrame = elapsed_seconds.count();
    }

    void switchAnim(int newAnimID) {
        if (animationType != newAnimID) {
            animationType = newAnimID;
            animStartTime = std::chrono::system_clock::now();
        }
    }
};

#endif
