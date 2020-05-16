#pragma once
#include "RenderController.h"
#include <Tool.hpp>
#include "Scene.h"
#include "HealthBar.h"

#define SEED_BAG_SCALER 0.2
#define WATER_CAN_SCALER 0.35
#define SHOVEL_SCALER 0.35

#define CAN_BAR_COLOR glm::vec3(0.1, 0.9, 1.0)
#define CAN_BAR_TRANSLATE_Y 1.3;

class ToolController : public RenderController {
public:
	ToolController(Tool * tool, Scene * scene) {
		rootNode = new SceneNode(NULL, "ToolRootEmpty", tool->objectId);
		barNode = NULL;
		particleNode = NULL;
		pGroup = NULL;
		filledBar;
		type = tool->toolType;
		holderId = 0;

		if (type == Tool::ToolType::WATER_CAN) { // TODO make this a constant
			modelNode = scene->getModel(ModelType::WATERING_CAN)->createSceneNodes(tool->objectId);
			modelNode->scaler = WATER_CAN_SCALER;

			float barTranslateY = CAN_BAR_TRANSLATE_Y;
			float initBarFilledFraction = 1.0f;
			glm::vec3 barColor = CAN_BAR_COLOR;
			filledBar = new HealthBar(
				scene->getShaderID(ShaderType::HEALTH_BAR),
				"texture/water_icon.png",
				barTranslateY, initBarFilledFraction, barColor
			);
			barNode = filledBar->createSceneNodes(rootNode->objectId);
			uiNodes.push_back(barNode);
			rootNode->addChild(barNode);
		}
		else if (type == Tool::ToolType::PLOW) {
			modelNode = scene->getModel(ModelType::SHOVEL)->createSceneNodes(tool->objectId);
			modelNode->scaler = SHOVEL_SCALER;
		}
		else if(type == Tool::ToolType::SEED) {
			modelNode = scene->getModel(ModelType::SEED_BAG)->createSceneNodes(tool->objectId);
			modelNode->scaler = SEED_BAG_SCALER;
		}
		else {
			modelNode = scene->getModel(ModelType::SEED_BAG)->createSceneNodes(tool->objectId);
			modelNode->scaler = SEED_BAG_SCALER;
		}
		rootNode->addChild(modelNode);
		scene->getGroundNode()->addChild(rootNode);
	}

	~ToolController() {
		if (filledBar) delete filledBar;
		if (pGroup) delete pGroup;
	}

	void update(GameObject * gameObject, Scene * scene) override {
		update((Tool*)gameObject, scene);
	}

	void update(Tool * tool, Scene * scene) {
		
		if (!tool->held) {
			rootNode->loadGameObject(tool);
			if (rootNode->parent != scene->getGroundNode()) {
				rootNode->scaler = 1.0;
				rootNode->pose = glm::vec3(0);
				scene->getGroundNode()->addChild(rootNode);
			}
		}
		if (tool->toolType == Tool::ToolType::WATER_CAN) { 
			if (tool->remainingWater == 0.0f) {
				filledBar->resetBar(0.0f);
			}
			else {
				filledBar->updateBar(tool->remainingWater / tool->capacity);
			}
			if (tool->held) {
				if (scene->objectIdMap.find(holderId) != scene->objectIdMap.end()) {
					barNode->pose[1] = -scene->objectIdMap[holderId]->pose[1];
				}
			}
			else {
				barNode->pose[1] = -rootNode->pose[1];
			}
		}
		else if (tool->toolType == Tool::ToolType::PLOW) {
			
		}
		else if (tool->toolType == Tool::ToolType::SEED) {
			
		}
	}

	void putInHand(SceneNode * handNode, float scaler, glm::vec3 holdVec, glm::vec3 holdPose, Scene * scene) {
		if (rootNode->parent != handNode) {
			holderId = handNode->objectId;
			handNode->addChild(rootNode);
			rootNode->scaler = 1.0/scaler;
			rootNode->position = holdVec;
			rootNode->pose = holdPose;
		}
	}

	Drawable * getModelFromType(Tool::ToolType type, Scene * scene) {
		switch (type) {
		case Tool::ToolType::PLOW:		return scene->getModel(ModelType::SHOVEL);
		case Tool::ToolType::WATER_CAN:	return scene->getModel(ModelType::WATERING_CAN);
		case Tool::ToolType::SEED:		return scene->getModel(ModelType::SEED_BAG);
		}
	}

	float getScalerFromType(Tool::ToolType type) {
		switch (type) {
			case Tool::ToolType::PLOW:		return SHOVEL_SCALER;
			case Tool::ToolType::WATER_CAN:	return WATER_CAN_SCALER;
			case Tool::ToolType::SEED:		return SEED_BAG_SCALER;
		}
		return 1.0;
	}

Tool::ToolType type;

private:
	uint holderId;
	HealthBar* filledBar;
	SceneNode* barNode;
	SceneNode* particleNode;
	ParticleGroup* pGroup;
};