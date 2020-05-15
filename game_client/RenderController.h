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

class Scene; //  put declaration here to sidestep header issues

class RenderController {
public:
    RenderController() {}

    SceneNode* rootNode;
    SceneNode* modelNode;

    virtual void update(GameObject* gameObject, Scene* scene) {};
private:
};
