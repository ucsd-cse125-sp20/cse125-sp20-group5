////////////////////////////////////////
// RenderController.h
// 
// Class that wraps the scene node and model of a game object
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "Constants.h"

#include "SceneNode.h"
#include "Drawable.h"


class RenderController {
public:
    RenderController() {}

    SceneNode* rootNode;
    SceneNode* modelNode;

    void virtual update(GameObject * gameObject, Scene * scene) = 0;
private:
};
