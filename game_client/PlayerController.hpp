#pragma once
#include "Scene.h"
#include "RenderController.hpp"
#include "ToolController.hpp"
#include "HealthBar.h"
#include "TextUI.h"

#define WATER_CAN_HOLD_VEC glm::vec3(-1.0, 0.0, 0.0)
#define SEED_BAG_HOLD_VEC glm::vec3(-1.0,-1.0,0.0)
#define SHOVEL_HOLD_VEC glm::vec3(0.0, 0.0, 0.0)
#define FELINE_SHOVEL_HOLD_VEC glm::vec3(-.6, 1.7, 2.1)
#define SPRAY_HOLD_VEC glm::vec3(-.5, -1.5, 0.3)
#define FERTILIZER_HOLD_VEC glm::vec3(-.5, .5, 0.3)

#define FERTILIZER_HOLD_ANGLE glm::vec3(3.14/2, 0, 0)
#define SHOVEL_HOLD_ANGLE glm::vec3(0,0,3.14/2)
#define FELINE_SHOVEL_HOLD_ANGLE glm::vec3(0.5,3.14/2,-2.8/2)

static constexpr float HP_BAR_TRANSLATE_Y = 1.9;
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
	static constexpr float CHAT_TEXT_TRANSLATE_Y = 2.9f; // black

	static constexpr glm::vec3 FELINE_PLOUGH_ANGLE = glm::vec3(3.14/2, 0, 3.14/2);

	const static std::string chatMessages[16];
public:
	PlayerController(Player* player, Scene* scene) {
		this->playerScaler = scene->config.playerScaler;

		// determine model type based on player ID
		this->playerId = player->playerId;
		switch (playerId % 4) {
			case 0: modelType = ModelType::CAT; break;
			case 1: modelType = ModelType::CHICKEN; break; 
			case 2: modelType = ModelType::TIGER; break;
			case 3: modelType = ModelType::BLACKPIG; break;
		}
		
		// create node
		rootNode = new SceneNode(NULL, "PlayerRootEmpty" + std::to_string(player->objectId), player->objectId);
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
		chatText->autoFadeOff = true;
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
			chatText->alphaValue = chatText->maxAlpha;
			// reset timer
			chatText->maxAlphaStartTime = std::chrono::system_clock::now(); // to allow new text be rendered for awhile
			chatText->reservedText = chatMessages[chatId];
		}

		// update the effect of textUI: 
		// should be handled by DrawableUI::update() when autoFadeOff turned on
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
					if (modelNode->obj == scene->getModel(ModelType::CAT) || modelNode->obj == scene->getModel(ModelType::TIGER)) {
						if (modelNode->animationId == Player::PlayerAnimation::PLOUGH)
							controller->putInHand(playerHand, playerScaler, glm::vec3(0), FELINE_PLOUGH_ANGLE, scene);
						else
							controller->putInHand(playerHand, playerScaler, FELINE_SHOVEL_HOLD_VEC, FELINE_SHOVEL_HOLD_ANGLE, scene);
					}
					else {
						controller->putInHand(playerHand, playerScaler, SHOVEL_HOLD_VEC, SHOVEL_HOLD_ANGLE, scene);
					}
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

const std::string PlayerController::chatMessages[16] = {
	"You suck", "Water", "Shovel", "Pesticide", "Fertilizer", // 0~4
	"Come on", "Help", "I'm so dead", "Thanks", "Good job", // 5~9
	"Thank you for the quarter!", // secret
	"Thank you for the quarter!", // alex Zhu
	"Thank you for the quarter!", // arun sUgUmar
	"Thank you for the quarter!", // Joyaan
	"Thank you for the quarter!", // Mingqi
	"Thank you for the quarter!" // Yang
};