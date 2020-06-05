#pragma once
#include "Scene.h"
#include "RenderController.hpp"

#define HOME_BASE_SCALER 0.15

class BaseController : public RenderController {
private:
	HealthBar* hpBar;
	SceneNode* barNode;

	static constexpr float HP_BAR_TRANSLATE_Y = 3.0;
	static constexpr glm::vec3 HP_BAR_COLOR = glm::vec3(0.3, 1.0, 0.4);

public:
	BaseController(HomeBase* homeBase, Scene* scene) {
		rootNode = new SceneNode(NULL, "HomeBaseRootEmpty", homeBase->objectId);
		modelNode = scene->getModel(ModelType::HOME_BASE)->createSceneNodes(homeBase->objectId);
		modelNode->scaler = HOME_BASE_SCALER;

		rootNode->addChild(modelNode);
		scene->getGroundNode()->addChild(rootNode);

		// init hp bar
		float initBarFilledFraction = 1.0f;
		HealthBarSetting barSetting(
			"texture/hp_icon.png", HP_BAR_TRANSLATE_Y, initBarFilledFraction, HP_BAR_COLOR
		);
		std::tie(hpBar, barNode) = createHealthBar(barSetting, scene);
	}

	~BaseController() {
		if (barNode) {
			barNode = RenderController::deleteBarNode(barNode);
		}
		if (hpBar) { delete hpBar; }
	}

	void update(GameObject* gameObject, Scene* scene) override {
		HomeBase* homeBase = (HomeBase*) gameObject;

		rootNode->loadGameObject(homeBase);

		updateHpBar(homeBase, scene);

		// update anim
		if (homeBase->animation->animationType == 2) {
			modelNode->switchAnim(homeBase->animation->animationType, false);
		}
		else {
			modelNode->switchAnim(homeBase->animation->animationType);
		}
	}

	void updateHpBar(HomeBase* homeBase, Scene* scene) {
		if (homeBase->maxHealth <= 0) {
			return; // data hasn't been received from server yet
		}

		if (hpBar->currFilledFraction <= 0.0) {
			if (barNode) {
				barNode = RenderController::deleteBarNode(barNode);
			}
		}
		else {
			hpBar->updateBar((float)homeBase->health / (float)homeBase->maxHealth);
		}
	}
};