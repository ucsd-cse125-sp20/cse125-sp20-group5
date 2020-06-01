#pragma once
#include "Scene.h"
#include "RenderController.hpp"
#include "ToolController.hpp"


#define PLAYER_SCALER 0.30
#define WATER_CAN_HOLD_VEC glm::vec3(-1.0, 0.0, 0.0)
#define SEED_BAG_HOLD_VEC glm::vec3(-1.0,-1.0,0.0)
#define SHOVEL_HOLD_VEC glm::vec3(0.0, 0.0, 0.0)
#define SPRAY_HOLD_VEC glm::vec3(-.5, -1.5, 0.3)
#define FERTILIZER_HOLD_VEC glm::vec3(-.5, .5, 0.3)

#define FERTILIZER_HOLD_ANGLE glm::vec3(3.14/2, 0, 0)
#define SHOVEL_HOLD_ANGLE glm::vec3(0,0,3.14/2)

class PlayerController : public RenderController {
private:
	uint playerId;
	ModelType modelType;

public:
	PlayerController(Player* player, Scene* scene) {
		// determine model type based on player ID
		this->playerId = player->playerId;
		switch (playerId % 4) {
			case 0: modelType = ModelType::TIGER; break;
			case 1: modelType = ModelType::CHICKEN; break; 
			case 2: modelType = ModelType::BLACKPIG; break;
			case 3: modelType = ModelType::CAT; break;
		}
		
		// create node
		rootNode = new SceneNode(NULL, "PlayerRootEmpty" + player->objectId, player->objectId);
		modelNode = scene->getModel(modelType)->createSceneNodes(player->objectId);
		rootNode->addChild(modelNode);
		rootNode->scaler = PLAYER_SCALER;
		scene->getGroundNode()->addChild(rootNode);
	}

	~PlayerController() {}

	void update(GameObject * gameObject, Scene * scene) override {
		Player* player = (Player*) gameObject;

		rootNode->loadGameObject(player);
		bool dontLoop = modelType == ModelType::CAT
			&& player->animation->animationType == Player::PlayerAnimation::PLOUGH;
		modelNode->switchAnim(player->animation->animationType, !dontLoop);

		handleHoldingAction(player, scene);
		handleHighlighting(player, scene);
	}

	// here is wehre we handle stuff like making sure they are holding another object
	void handleHoldingAction(Player* player, Scene* scene) {
		if (!player->holding) {
			return;
		}

		if (scene->controllers.count(player->heldObject) > 0) {
			ToolController* controller = (ToolController*)(scene->controllers[player->heldObject]);
			SceneNode* playerHand = modelNode->find("j_r_hand", modelNode->objectId);
			if (playerHand != NULL) {
				if (controller->type == Tool::ToolType::WATER_CAN) {
					controller->putInHand(playerHand, PLAYER_SCALER, WATER_CAN_HOLD_VEC, glm::vec3(0), scene);
				}
				else if (controller->type == Tool::ToolType::PLOW) {
					controller->putInHand(playerHand, PLAYER_SCALER, SHOVEL_HOLD_VEC, SHOVEL_HOLD_ANGLE, scene);
				}
				else if (controller->type == Tool::ToolType::SEED) {
					controller->putInHand(playerHand, PLAYER_SCALER, SEED_BAG_HOLD_VEC, glm::vec3(0), scene);
				}
				else if (controller->type == Tool::ToolType::PESTICIDE) {
					controller->putInHand(playerHand, PLAYER_SCALER, SPRAY_HOLD_VEC, glm::vec3(0), scene);
				}
				else if (controller->type == Tool::ToolType::FERTILIZER) {
					controller->putInHand(playerHand, PLAYER_SCALER, FERTILIZER_HOLD_VEC, FERTILIZER_HOLD_ANGLE, scene);
				}
			}
		}
	}

	void handleHighlighting(Player* player, Scene* scene) {
		// Read the objectID for highlighting
		uint hightlightObjectId = player->highlightObjectId;
		// Set highlight

		if (scene->controllers.find(hightlightObjectId) != scene->controllers.end()) {
			scene->controllers[hightlightObjectId]->modelNode->highlighted = true;
		}
		else if (scene->objectIdMap.find(hightlightObjectId) != scene->objectIdMap.end()) {
			scene->objectIdMap[hightlightObjectId]->highlighted = true;
		}

		// Set highlight tiles
		((Ground*)(scene->getGroundNode()->obj))->highlightTile(player->highlightTileCol, player->highlightTileRow);
	}
};