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

#define SEED_SCALER 0.03
#define SEED_VEC glm::vec3(0.0, 0.2, 0.0)
#define SAPLING_SCALER 0.06

#define BABY_CORN_SCALER 0.3
#define CORN_SCALER 0.45
#define CORN_PARTICLE_HEIGHT glm::vec3(0, 2, 0)

#define BAR_COLOR glm::vec3(0.1, 0.9, 1.0)
#define BAR_TRANSLATE_Y 1.3;

class PlantController : public RenderController {
public:
    PlantController(Plant * plant, Scene* scene) {
        // init node
        rootNode = new SceneNode(NULL, "PlantRootEmptyNode", plant->objectId);
        modelNode = scene->getModel(ModelType::SEED)->createSceneNodes(plant->objectId);
        modelNode->scaler = SEED_SCALER;
        modelNode->position = SEED_VEC;

        rootNode->addChild(modelNode);
        scene->getGroundNode()->addChild(rootNode); 
        currGrowStage = Plant::GrowStage::SEED;


        // init growth bar
        float barTranslateY = BAR_TRANSLATE_Y;
        float initBarFilledFraction = 1.0f;
        glm::vec3 barColor = BAR_COLOR;
        growthBar = new HealthBar(
            scene->getShaderID(ShaderType::HEALTH_BAR),
            "texture/water_icon.png", 
            barTranslateY, initBarFilledFraction, barColor
        ); 
        barNode = growthBar->createSceneNodes(rootNode->objectId);
        rootNode->addChild(barNode);
        uiNodes.push_back(barNode);
    }

    ~PlantController() {
        // TODO: (not really tho) ideally we wouldn't need this but then we have to refactor particle group 
        // and growthbar and sceneNode
        // as of right now we need a new model for each instance which isn't great but eh what can you do?
        delete growthBar;
        delete pGroup;
    }

    void update(GameObject * gameObject, Scene* scene) override {
        // TODO maybe do some ceck here to see if we can cast
        update(((Plant*)gameObject), scene);
    }

    void update(Plant* plant, Scene* scene) {
        // Update grown plant model
        if (currGrowStage != plant->growStage) {
            updatePlantModel(plant, scene);
            currGrowStage = plant->growStage;
        }

        // Load new data provided by server
        rootNode->loadGameObject(plant);

        // Update growth bar
        if (plant->growStage == Plant::GrowStage::GROWN) {
            if (barNode) {
                barNode->removeSelf();
                uiNodes.erase(std::find(uiNodes.begin(), uiNodes.end(), barNode));
                barNode = nullptr;
            }
        }
        else {
            if (plant->growProgressTime == 0.0f) {
                growthBar->resetBar(0.0f);
            }
            else {
                growthBar->updateBar(plant->growProgressTime / plant->growExpireTime);
            }
        }
    }

    void updatePlantModel(Plant* plant, Scene* scene) {
        // Delete old model node
        delete modelNode;

        // Switch to new model
        uint objectId = plant->objectId;
        switch (plant->growStage) {
            case Plant::GrowStage::SEED:
                modelNode = scene->getModel(ModelType::SEED)->createSceneNodes(objectId);
                modelNode->scaler = SEED_SCALER;
                modelNode->position = SEED_VEC;
                break;
            case Plant::GrowStage::SAPLING:
                modelNode = scene->getModel(ModelType::SAPLING)->createSceneNodes(objectId);
                modelNode->scaler = SAPLING_SCALER;
                break;
            case Plant::GrowStage::BABY:
                if (plant->plantType == Plant::PlantType::CORN) {
                    modelNode = scene->getModel(ModelType::BABY_CORN)->createSceneNodes(objectId);
                    modelNode->scaler = BABY_CORN_SCALER;
                }
                break;
            case Plant::GrowStage::GROWN:
                if (plant->plantType == Plant::PlantType::CORN) {
                    modelNode = scene->getModel(ModelType::CORN)->createSceneNodes(objectId);
                    modelNode->scaler = CORN_SCALER;
                    float attackRange = plant->range->rangeDistance;
                    pGroup = scene->getParticleFactory()->getCornAttackParticleGroup(glm::vec3(0, 0, 0), attackRange);
                    particleNode = pGroup->createSceneNodes(plant->objectId);
                    particleNode->position = CORN_PARTICLE_HEIGHT;
                    modelNode->addChild(particleNode);
                }
                break;
        }
        rootNode->addChild(modelNode);
    }
    
private:
    HealthBar* growthBar;

    SceneNode* barNode;
    SceneNode* particleNode;
    ParticleGroup* pGroup; 
    Plant::GrowStage currGrowStage;
};