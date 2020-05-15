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
#include "HealthBar.h"

struct z_compare {
    bool operator() (const SceneNode* lhs, const SceneNode* rhs) const {
        return lhs->position.z < rhs->position.z;
    }
};

class RenderController {
public:
    RenderController() {}

    SceneNode* rootNode;
    SceneNode* modelNode;
    
    static std::set<SceneNode*, z_compare> uiNodes;
    static std::unordered_map<uint, RenderController*> controllerMap;

    static void drawUI(const glm::mat4& viewProjMtx) {
        HealthBar::canDraw = true;
        for (SceneNode* uiNode : uiNodes) {
            uiNode->draw(viewProjMtx);
        }
        HealthBar::canDraw = false;
    }

private:
};

std::set<SceneNode*, z_compare> RenderController::uiNodes;
std::unordered_map<uint, RenderController*> RenderController::controllerMap;