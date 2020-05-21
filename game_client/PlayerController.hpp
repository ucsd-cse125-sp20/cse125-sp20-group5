#pragma once
#include "Scene.h"
#include "RenderController.h"
#include "ToolController.h"


#define PLAYER_SCALER 0.30
#define WATER_CAN_HOLD_VEC glm::vec3(-1.0, 0.0, 0.0)
#define SEED_BAG__HOLD_VEC glm::vec3(-3.0,0.0,0.0)
#define SHOVEL_HOLD_VEC glm::vec3(0.0, 0.0, 0.0)
#define SHOVEL_HOLD_ANGLE glm::vec3(0,0,3.14/2)
#define CAT_ARM "j_r_arm_$AssimpFbx$_Translation"

class PlayerController : public RenderController {
public:
	PlayerController(Player* player, Scene* scene) {
		rootNode = new SceneNode(NULL, "PlaeryRootEmpty", player->objectId);
		modelNode = scene->getModel(ModelType::CAT)->createSceneNodes(player->objectId);
		rootNode->addChild(modelNode);
		rootNode->scaler = PLAYER_SCALER;
		scene->getGroundNode()->addChild(rootNode);
	}

	void update(GameObject * gameObject, Scene * scene) override {
		update((Player*)gameObject, scene);
	}

	void update(Player* player, Scene* scene) {
		// here is wehre we handle stuff like making sure they are holding another object
		rootNode->loadGameObject(player);
		if (player->holding) {
			if (scene->controllers.count(player->heldObject) > 0) {
				ToolController* controller = (ToolController*)(scene->controllers[player->heldObject]);
				SceneNode* playerHand = modelNode->findHand(modelNode->objectId);
				if (playerHand != NULL) {
					if (controller->type == Tool::ToolType::WATER_CAN) {
						controller->putInHand(playerHand, PLAYER_SCALER, WATER_CAN_HOLD_VEC, glm::vec3(0), scene);
					}
					else if (controller->type == Tool::ToolType::PLOW) {
						controller->putInHand(playerHand, PLAYER_SCALER, SHOVEL_HOLD_VEC, SHOVEL_HOLD_ANGLE, scene);
					}
					else if (controller->type == Tool::ToolType::SEED) {
						controller->putInHand(playerHand, PLAYER_SCALER, SEED_BAG__HOLD_VEC, glm::vec3(0), scene);
					}
				}
			}
		}

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