#pragma once

// TODO: the code for HealthBar and TextUI are not the best implementation 
// as they use static instead of a controller; I would say the best implementation
// is to let derived class of scene node become the controller for different types 
// of model, although requires plenty of refactoring

#include "Core.h"
#include "Constants.h"
#include "SceneNode.h"
#include "Drawable.h"
#include "stb_image.h"

class DrawableUI : public Drawable {
public:
    // Render decision related
    static bool isDrawUiMode; // Used to postpone drawing to the last for the sake of blending
    bool shouldDisplay = true;

    // alpha related
    bool alphaEffectOn = false;
    static constexpr float DEFAULT_MAX_ALPHA = 0.8f;
    float maxAlpha = DEFAULT_MAX_ALPHA;
    float alphaValue = DEFAULT_MAX_ALPHA;
    float alphaStep = 0.05f;

    bool autoFadeOff = false; // if true, will fade off after awhile automatically
    std::chrono::system_clock::time_point maxAlphaStartTime; // to allow the hpBar display for awhile
    static constexpr int RENDER_TIME_MILLISEC = 1500;


    DrawableUI();
    //virtual ~DrawableUI() {}

    void setAlphaSetting(bool alphaEffectOn, float initialAlpha, float alphaStep);
    bool canDraw();
    void loadTexture(const char* textureFile, GLuint* textureID);

    SceneNode* createSceneNodes(uint objectId) override;
    virtual void update(SceneNode* node) override;

private:
};
