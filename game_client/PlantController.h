////////////////////////////////////////
// PlantController.h
// 
// Class that wraps and controls the rendering attributes of Plant
////////////////////////////////////////

#pragma once

#include "RenderController.h"
#include <Plant.hpp>
#include "HealthBar.h" //TODO to be removed
#include "Scene.h"


class PlantController : public RenderController {
public:
    PlantController(uint objectId, Scene* scene) {
        // init node
        rootNode = scene->getModel(ModelType::CORN)->createSceneNodes(objectId);
        scene->getGroundNode()->addChild(rootNode); 

        rootNode->scaler = .5;
        rootNode->scaler = RABBIT_SCALER;
        rootNode->position[1] = .7;

        // init growth bar
        float barTranslateY = 2.0f;
        float initBarFilledFraction = 1.0f;
        glm::vec3 barColor = glm::vec3(0.1, 0.9, 1.0);
        growthBar = new HealthBar(
            scene->getShaderID(ShaderType::HEALTH_BAR),
            "texture/water_icon.png", 
            barTranslateY, initBarFilledFraction, barColor
        ); 
        barNode = growthBar->createSceneNodes(rootNode->objectId);
        rootNode->addChild(barNode);
    }

    static PlantController* getController(uint objectId, Scene* scene) {
        if (controllerMap.find(objectId) == controllerMap.end()) {
            controllerMap[objectId] = new PlantController(objectId, scene); // TODO we should have an intialization network connection to do this
        }
        return (PlantController*) controllerMap[objectId];
    }

    void update(Plant* plant) {
        // Load new data provided by server
        rootNode->loadGameObject(plant);

        // Update growth bar
        if (plant->growStage == Plant::GrowStage::GROWN) {
            barNode->removeSelf();
        }
        else if (plant->growProgressTime == 0.0f) {
            growthBar->resetBar(0.0f);
        }
        else {
            growthBar->updateBar(plant->growProgressTime / plant->growExpireTime);
        }
    }
    
private:
    HealthBar* growthBar;
    SceneNode* barNode;
};