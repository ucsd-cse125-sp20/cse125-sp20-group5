#pragma once
#include "Scene.h"
#include "RenderController.h"

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

		// init growth bar
		float initBarFilledFraction = 1.0f;
		HealthBarSetting barSetting(
			"texture/hp_icon.png", HP_BAR_TRANSLATE_Y, initBarFilledFraction, HP_BAR_COLOR
		);
		std::tie(hpBar, barNode) = createHealthBar(barSetting, scene);
	}

	void update(GameObject* gameObject, Scene* scene) override {
		update((HomeBase*)gameObject, scene);
	}

	void update(HomeBase* homeBase, Scene* scene) {
		rootNode->loadGameObject(homeBase);

		// Update growth bar
		if (homeBase->maxHealth <= 0) {
			return; // data hasn't been received from server yet
		}
		if (homeBase->health <= 0) {
			if (barNode) {
				uiNodes.erase(std::find(uiNodes.begin(), uiNodes.end(), barNode));
				barNode = nullptr;
			}
		}
		else {	
			hpBar->updateBar((float)homeBase->health / (float)homeBase->maxHealth);
		}
	}

};