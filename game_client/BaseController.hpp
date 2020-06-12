#pragma once
#include "Scene.h"
#include "RenderController.hpp"

#define HOME_BASE_SCALER 0.15

class BaseController : public RenderController {
private:
	HealthBar* hpBar;
	SceneNode* barNode;

	int prevMaxHealth;
	int prevHealth;

	static constexpr float HP_BAR_TRANSLATE_Y = 3.0;
	static constexpr glm::vec3 HP_BAR_COLOR = glm::vec3(0.3, 1.0, 0.4);

public:
	BaseController(HomeBase* homeBase, Scene* scene) {
		rootNode = new SceneNode(NULL, "HomeBaseRootEmpty", homeBase->objectId);
		modelNode = scene->getModel(ModelType::HOME_BASE)->createSceneNodes(homeBase->objectId);
		modelNode->scaler = HOME_BASE_SCALER;
		modelNode->loadAnimData(modelNode->numAnimation, HomeBase::HomeBaseAnimation::STAY);

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
			barNode = deleteUiNode(barNode);
		}
		if (hpBar) { delete hpBar; }
	}

	void update(GameObject* gameObject, Scene* scene) override {
		HomeBase* homeBase = (HomeBase*) gameObject;

		rootNode->loadGameObject(homeBase);

		updateAnimationAndAudio(homeBase, scene);

		updateHpBar(homeBase, scene);

		// updated at the end
		this->prevHealth = homeBase->health;
		this->prevMaxHealth = homeBase->maxHealth;
	}

	// for base, animation is handled by client due to lack of time, needs refactor
	void updateAnimationAndAudio(HomeBase* homeBase, Scene* scene) {
		if (homeBase->maxHealth <= 0) {
			modelNode->switchAnim(HomeBase::HomeBaseAnimation::STAY);
			return; // data hasn't been received from server yet
		}

		int newAnimID = homeBase->animation->animationType;
		switch (newAnimID) {
		case HomeBase::HomeBaseAnimation::DIE:
			modelNode->switchAnim(newAnimID, false);
			if (this->prevHealth != homeBase->health) {
				scene->aEngine->PlaySounds(AUDIO_FILE_HOMEBASE_DIE, glm::vec3(rootNode->transform[3]),
					scene->aEngine->VolumeTodB(scene->volumeAdjust * 1.5f));
			}
			break;
		case HomeBase::HomeBaseAnimation::DAMAGED:
			modelNode->switchAnim(newAnimID, false);
			scene->aEngine->PlaySounds(AUDIO_FILE_HOMEBASE_DAMAGED, glm::vec3(rootNode->transform[3]),
				scene->aEngine->VolumeTodB(scene->volumeAdjust * 1.5f));
			break;
		case HomeBase::HomeBaseAnimation::STAY:
			if (modelNode->playedOneAnimCycle) {
				modelNode->switchAnim(newAnimID);
			}
			break;
		}
	}

	void updateHpBar(HomeBase* homeBase, Scene* scene) {
		if (homeBase->maxHealth <= 0) {
			return; // data hasn't been received from server yet
		}

		if (hpBar->currFilledFraction <= 0.0) {
			if (barNode) {
				barNode = deleteUiNode(barNode);
			}
		}
		else {
			hpBar->updateBar((float)homeBase->health / (float)homeBase->maxHealth);
		}
	}
};