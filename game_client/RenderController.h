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

    static std::unordered_map<uint, RenderController*> controllerMap;

private:
};

std::unordered_map<uint, RenderController*> RenderController::controllerMap;