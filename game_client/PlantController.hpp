////////////////////////////////////////
// PlantController.hpp
// 
// Class that wraps and controls the rendering attributes of Plant
////////////////////////////////////////

#pragma once

#include "RenderController.hpp"
#include <Plant.hpp>
#include "HealthBar.h" //TODO to be removed
#include "Scene.h"

#define SEED_SCALER 0.10
#define SEED_VEC glm::vec3(0.0, 0.2, 0.0)
#define SAPLING_SCALER 0.25

#define BABY_CORN_SCALER 0.45
#define CORN_SCALER 0.45
#define CORN_PARTICLE_HEIGHT glm::vec3(0, 2, 0)

#define BABY_CACTUS_SCALER 0.3
#define CACTUS_SCALER 0.45

#define BUG_ANIMATION 2
#define ATTACK_ANIMATION 1
#define IDLE_ANIMATION 0

class PlantController : public RenderController {

private:
    HealthBar* growthBar;
    HealthBar* coolDownBar;
    HealthBar* hpBar;
    HealthBar* pesticideBar;
    HealthBar* fertilizeBar;
    SceneNode* gBarNode;
    SceneNode* cBarNode;
    SceneNode* hBarNode;
    SceneNode* pBarNode;
    SceneNode* fBarNode;

    ParticleGroup* pGroup;
    SceneNode* particleNode;

    Plant::GrowStage currGrowStage;

    static constexpr float WATERING_BAR_TRANSLATE_Y = 1.3;
    static constexpr glm::vec3 WATERING_BAR_COLOR = glm::vec3(0.1, 0.9, 1.0); // blue
    static constexpr float COOLDOWN_BAR_TRANSLATE_Y = 1.3;
    static constexpr glm::vec3 COOLDOWN_BAR_COLOR = glm::vec3(0.6, 0.6, 0.6); // grey
    static constexpr float HP_BAR_TRANSLATE_Y = 2.0;
    static constexpr glm::vec3 HP_BAR_COLOR = glm::vec3(0.3, 1.0, 0.4); // green
    static constexpr glm::vec3 PESTICIDE_BAR_COLOR = glm::vec3(0.8, 0.3, 1.0); // purple
    static constexpr glm::vec3 FERTILIZE_BAR_COLOR = glm::vec3(1.0, 0.5, 0.0); // orange

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
        float initBarFilledFraction = 1.0f;
        HealthBarSetting gBarSetting("texture/water_icon.png", WATERING_BAR_TRANSLATE_Y, initBarFilledFraction, WATERING_BAR_COLOR);
        std::tie(growthBar, gBarNode) = createHealthBar(gBarSetting, scene);

        // init coolOff bar
        initBarFilledFraction = 0.0f;
        HealthBarSetting cBarSetting("texture/time_icon.png", COOLDOWN_BAR_TRANSLATE_Y, initBarFilledFraction, COOLDOWN_BAR_COLOR);
        std::tie(coolDownBar, cBarNode) = createHealthBar(cBarSetting, scene);
        coolDownBar->shouldDisplay = false;

        // init hp bar
        initBarFilledFraction = 1.0f;
        HealthBarSetting hBarSetting("texture/hp_icon.png", HP_BAR_TRANSLATE_Y, initBarFilledFraction, HP_BAR_COLOR);
        std::tie(hpBar, hBarNode) = createHealthBar(hBarSetting, scene);
        hpBar->shouldDisplay = false;

        // init pesticide bar
        initBarFilledFraction = 1.0f;
        float barMarginY = HealthBar::BAR_HEIGHT / 2.0f;
        HealthBarSetting pBarSetting("texture/bug_icon.png", HP_BAR_TRANSLATE_Y + barMarginY, initBarFilledFraction, PESTICIDE_BAR_COLOR);
        std::tie(pesticideBar, pBarNode) = createHealthBar(pBarSetting, scene);
        pesticideBar->shouldDisplay = false;

        // init fertilize bar
        initBarFilledFraction = 0.0f;
        HealthBarSetting fBarSetting("texture/plus_icon.png", HP_BAR_TRANSLATE_Y + 2.0 * barMarginY, initBarFilledFraction, FERTILIZE_BAR_COLOR);
        std::tie(fertilizeBar, fBarNode) = createHealthBar(fBarSetting, scene);
        fertilizeBar->shouldDisplay = false;
        fertilizeBar->fillingStep /= 5.0f;
        fertilizeBar->alphaEffectOn = true;
        fertilizeBar->alphaValue = 0.0f;
        fertilizeBar->alphaStep *= 1.0f;
    }

    ~PlantController() {
        if (gBarNode) { gBarNode = RenderController::deleteBarNode(gBarNode); }
        if (growthBar) { delete growthBar; }
        if (cBarNode) { cBarNode = RenderController::deleteBarNode(cBarNode); }
        if (coolDownBar) { delete coolDownBar; }
        if (hBarNode) { hBarNode = RenderController::deleteBarNode(hBarNode); }
        if (hpBar) { delete hpBar; }
        if (pBarNode) { pBarNode = RenderController::deleteBarNode(pBarNode); }
        if (pesticideBar) { delete pesticideBar; }
        if (fBarNode) { fBarNode = RenderController::deleteBarNode(fBarNode); }
        if (fertilizeBar) { delete fertilizeBar; }

        delete pGroup;
    }

    void update(GameObject * gameObject, Scene* scene) override {
        Plant* plant = (Plant*) gameObject;

        // Update grown plant model
        if (currGrowStage != plant->growStage) {
            updatePlantModel(plant, scene);
            currGrowStage = plant->growStage;
        }

        // Load new data provided by server
        rootNode->loadGameObject(plant);

        // Update growth bar
        updateGrowthBar(plant, scene);

        // Update animation and particle effect
        updateAnimation(plant);
    }

    void updatePlantModel(Plant* plant, Scene* scene) {
        // Delete old model node
        modelNode->removeSelf();
        delete modelNode;

        // Switch to new model
        uint objectId = plant->objectId;
        switch (plant->growStage) {
            case Plant::GrowStage::SEED:
                if (plant->plantType == Plant::PlantType::PLAYER) {
                    modelNode = scene->getModel(ModelType::BABY_PLAYER_PLANT)->createSceneNodes(objectId);
                }
                else {
                    modelNode = scene->getModel(ModelType::SEED)->createSceneNodes(objectId);
                }
                modelNode->scaler = SEED_SCALER;
                modelNode->position = SEED_VEC;
                break;
            case Plant::GrowStage::SAPLING:
                if (plant->plantType == Plant::PlantType::PLAYER) {
                    modelNode = scene->getModel(ModelType::BABY_PLAYER_PLANT)->createSceneNodes(objectId);
                }
                else {
                    modelNode = scene->getModel(ModelType::SAPLING)->createSceneNodes(objectId);
                }
                modelNode->scaler = SAPLING_SCALER;
                break;
            case Plant::GrowStage::BABY:
                if (plant->plantType == Plant::PlantType::CORN) {
                    modelNode = scene->getModel(ModelType::BABY_CORN)->createSceneNodes(objectId);
                    modelNode->scaler = BABY_CORN_SCALER;
                }
                else if (plant->plantType == Plant::PlantType::CACTUS) {
                    modelNode = scene->getModel(ModelType::BABY_CACTUS)->createSceneNodes(objectId);
                    modelNode->scaler = BABY_CACTUS_SCALER;
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
                else if (plant->plantType == Plant::PlantType::CACTUS) {
                    modelNode = scene->getModel(ModelType::CACTUS)->createSceneNodes(objectId);
                    modelNode->scaler = CACTUS_SCALER;
                }
                break;
        }
        rootNode->addChild(modelNode);
    }

    void updateGrowthBar(Plant* plant, Scene* scene) {
        if (plant->growStage == Plant::GrowStage::GROWN) {
            // delete all growth-related bar
            if (gBarNode) { gBarNode = RenderController::deleteBarNode(gBarNode); }
            if (cBarNode) { cBarNode = RenderController::deleteBarNode(cBarNode); }

            // update hp bar
            hpBar->shouldDisplay = true;
            hpBar->updateBar((plant->deathTime - plant->aliveTime) / plant->deathTime);

            // update pesticide bar
            if (plant->isAttackedByBugs) {
                pesticideBar->shouldDisplay = true;
                pesticideBar->updateBar((plant->pesticideSprayTime - plant->currSprayTime) / plant->pesticideSprayTime);
            }
            else {
                pesticideBar->shouldDisplay = false;
                pesticideBar->resetBar(1.0f);
            }

            // update fertilize bar
            float newFilledFraction = plant->currFertilizeTime / plant->fertilizerCompleteTime;
            if (fertilizeBar->currFilledFraction == newFilledFraction) {
                fertilizeBar->shouldDisplay = false;
            }
            else {
                fertilizeBar->shouldDisplay = true;
                fertilizeBar->updateBar(newFilledFraction);
            }
            if (newFilledFraction == 0.0) { fertilizeBar->resetBar(0.0f); }
        }
        else {
            if (plant->cooldownTime > 0) {
                if (!coolDownBar->shouldDisplay) {
                    coolDownBar->resetBar(1.0f);
                }
                coolDownBar->updateBar(plant->cooldownTime / plant->coolDownExpireTime);

                coolDownBar->shouldDisplay = true;
                growthBar->shouldDisplay = false;
            }
            else {
                if (plant->growProgressTime == 0.0f) {
                    growthBar->resetBar(0.0f);
                }
                growthBar->updateBar(plant->growProgressTime / plant->growExpireTime);

                coolDownBar->shouldDisplay = false;
                growthBar->shouldDisplay = true;    
            }
        }
    }

    void updateAnimation(Plant* plant) {
        // Check if is attacked by the bugs
        if (plant->isAttackedByBugs && this->modelNode->animationId != BUG_ANIMATION) {
            this->modelNode->switchAnim(BUG_ANIMATION, true);
        }

        // Realse particles if necessary
        if (!plant->isAttackedByBugs &&
            pGroup != NULL && plant->currAttackTime >= plant->attackInterval) {
            pGroup->releaseParticles();
            this->modelNode->switchAnim(ATTACK_ANIMATION, false);
        }

        // Reset back to idle if
        // 1. played one attack animation or
        // 2. attacked by bugs ended
        if (this->modelNode->animationId == ATTACK_ANIMATION &&
            this->modelNode->playedOneAnimCycle ||
            this->modelNode->animationId == BUG_ANIMATION &&
            !plant->isAttackedByBugs) {
            this->modelNode->switchAnim(IDLE_ANIMATION, true);
        }
    }
};