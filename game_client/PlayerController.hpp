#pragma once
#include "Scene.h"
#include "RenderController.hpp"
#include "ToolController.hpp"
#include "HealthBar.h"
#include "TextUI.h"

#define WATER_CAN_HOLD_VEC glm::vec3(-1.0, 0.0, 0.0)
#define SEED_BAG_HOLD_VEC glm::vec3(-1.0,-1.0,0.0)
#define SHOVEL_HOLD_VEC glm::vec3(0.0, 0.0, 0.0)
#define SPRAY_HOLD_VEC glm::vec3(-.5, -1.5, 0.3)
#define FERTILIZER_HOLD_VEC glm::vec3(-.5, .5, 0.3)

#define FERTILIZER_HOLD_ANGLE glm::vec3(3.14/2, 0, 0)
#define SHOVEL_HOLD_ANGLE glm::vec3(0,0,3.14/2)

static constexpr float HP_BAR_TRANSLATE_Y = 2.3;
static constexpr glm::vec3 HP_BAR_COLOR = glm::vec3(0.3, .7, 0.4);

class PlayerController : public RenderController {
private:
	uint playerId;
	ModelType modelType;
	HealthBar* hpBar;
	SceneNode * barNode;
	float playerScaler;

	TextUI* chatText;
	SceneNode* textNode;

	static constexpr glm::vec3 CHAT_TEXT_COLOR = glm::vec3(0); // black
	static constexpr float CHAT_TEXT_TRANSLATE_Y = 2.0f; // black

public:
	PlayerController(Player* player, Scene* scene) {
		this->playerScaler = scene->config.playerScaler;

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
		rootNode->scaler = playerScaler;
		scene->getGroundNode()->addChild(rootNode);

		// init hp bar
		float initBarFilledFraction = 1.0f;
		HealthBarSetting barSetting(
			"texture/hp_icon.png", HP_BAR_TRANSLATE_Y, initBarFilledFraction, HP_BAR_COLOR
		);
		std::tie(hpBar, barNode) = createHealthBar(barSetting, scene);

		// init chat text
		std::tie(chatText, textNode) = createTextUI(
			FontType::CHUNK, CHAT_TEXT_COLOR,
			glm::translate(glm::vec3(0, CHAT_TEXT_TRANSLATE_Y, 0)), scene
		);
		chatText->shouldDisplay = false;
		chatText->setAlphaSetting(true, 0.0f, chatText->alphaStep);
	}

	~PlayerController() {
		if (barNode) { barNode = RenderController::deleteBarNode(barNode); }
		if (hpBar) { delete hpBar; }
		if (textNode) { textNode = RenderController::deleteBarNode(textNode); }
		if (chatText) { delete chatText; }
	}

	void update(GameObject * gameObject, Scene * scene) override {
		Player* player = (Player*) gameObject;
		
		if (player->isDead) {
			if (modelNode != nullptr) {
				modelNode->removeSelf();
			}
		}
		if (!player->isDead) {
			//modelNode = scene->getModel(modelType)->createSceneNodes(player->objectId);
			if (modelNode->childNum == -1)
				rootNode->addChild(modelNode);
		}

		updateChat(player);

		rootNode->loadGameObject(player);
		bool dontLoop = (modelType == ModelType::CAT || modelType == ModelType::TIGER)
			&& player->animation->animationType == Player::PlayerAnimation::PLOUGH;
		modelNode->switchAnim(player->animation->animationType, !dontLoop);

		handleHoldingAction(player, scene);
		handleHighlighting(player, scene);
		updateHpBar(player, scene);
	}

	void updateChat(Player* player) {
		// change the text content, if player object has a valid chatId
		int chatId = player->currChat;
		if (chatId != Player::NO_CHAT) {
			chatText->shouldDisplay = true;
			// reset timer
			//TODO
			chatText->reservedText = Player::chatMessages[chatId];
		}

		// update the textUI
		// TODO
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
					controller->putInHand(playerHand, playerScaler, WATER_CAN_HOLD_VEC, glm::vec3(0), scene);
				}
				else if (controller->type == Tool::ToolType::PLOW) {
					controller->putInHand(playerHand, playerScaler, SHOVEL_HOLD_VEC, SHOVEL_HOLD_ANGLE, scene);
				}
				else if (controller->type == Tool::ToolType::SEED) {
					controller->putInHand(playerHand, playerScaler, SEED_BAG_HOLD_VEC, glm::vec3(0), scene);
				}
				else if (controller->type == Tool::ToolType::PESTICIDE) {
					controller->putInHand(playerHand, playerScaler, SPRAY_HOLD_VEC, glm::vec3(0), scene);
				}
				else if (controller->type == Tool::ToolType::FERTILIZER) {
					controller->putInHand(playerHand, playerScaler, FERTILIZER_HOLD_VEC, FERTILIZER_HOLD_ANGLE, scene);
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

	void updateHpBar(Player* player, Scene* scene) {
		if (player->maxHealth <= 0) {
			return; // data hasn't been received from server yet
		}
		hpBar->shouldDisplay = !player->isDead;
		hpBar->updateBar((float)player->health / (float)player->maxHealth);
	}
};