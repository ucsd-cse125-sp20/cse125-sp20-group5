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
        return lhs->transform[3].z < rhs->transform[3].z;
    }
};

struct HealthBarSetting {
    const char* iconFile;
    float translateY;
    float initFilledFraction;
    glm::vec3 barColor;

    HealthBarSetting(const char* iconFile, float translateY, float initFilledFraction, glm::vec3 barColor)
        : iconFile(iconFile), translateY(translateY), initFilledFraction(initFilledFraction), barColor(barColor) {}
};

class RenderController {
public:
    RenderController() {}

    SceneNode* rootNode;
    SceneNode* modelNode;


    virtual void update(GameObject* gameObject, Scene* scene) {};
   
    static std::vector<SceneNode*> uiNodes;

    static void drawUI(const glm::mat4& viewProjMtx) {
        std::sort(uiNodes.begin(), uiNodes.end(), z_compare());
        HealthBar::canDraw = true;
        for (SceneNode* uiNode : uiNodes) {
            uiNode->draw(viewProjMtx);
        }
        HealthBar::canDraw = false;
    }

    // Init the healthbar object and add its node to the scene graph and the uiNode vector
    std::pair<HealthBar*, SceneNode*> createHealthBar(HealthBarSetting barSetting, Scene* scene) {
        HealthBar* barObject = new HealthBar(
            scene->getShaderID(ShaderType::HEALTH_BAR),
            barSetting.iconFile, barSetting.translateY, barSetting.initFilledFraction, barSetting.barColor
        );
        SceneNode* barNode = barObject->createSceneNodes(rootNode->objectId);
        this->rootNode->addChild(barNode);
        uiNodes.push_back(barNode);

        return std::pair<HealthBar*, SceneNode*>(barObject, barNode);
    }

private:
};

std::vector<SceneNode*> RenderController::uiNodes;