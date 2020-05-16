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


class Scene; //  put declaration here to sidestep header issues

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


    virtual void update(GameObject* gameObject, Scene* scene) {};
   
    static std::set<SceneNode*, z_compare> uiNodes;

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