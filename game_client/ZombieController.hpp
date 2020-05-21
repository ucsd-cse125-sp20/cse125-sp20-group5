#pragma once
#include "Scene.h"
#include "RenderController.hpp"

class ZombieController : public RenderController {
private:
	HealthBar* hpBar;
	SceneNode* barNode;

	static constexpr float RABBIT_SCALER = 0.40;
	static constexpr float HP_BAR_TRANSLATE_Y = 1.7;
	static constexpr glm::vec3 HP_BAR_COLOR = glm::vec3(1.0, 0.2, 0.2);

public:
	ZombieController(Zombie* zombie, Scene* scene) {
		rootNode = new SceneNode(NULL, "ZombieRootEmpty" + zombie->objectId, zombie->objectId);
		modelNode = scene->getModel(ModelType::RABBIT)->createSceneNodes(zombie->objectId);
		modelNode->loopAnimation = false;
		modelNode->scaler = RABBIT_SCALER;

		rootNode->addChild(modelNode);
		scene->getGroundNode()->addChild(rootNode);

		// init hp bar
		float initBarFilledFraction = 1.0f;
		HealthBarSetting barSetting(
			"texture/hp_icon.png", HP_BAR_TRANSLATE_Y, initBarFilledFraction, HP_BAR_COLOR
		);
		//std::tie(hpBar, barNode) = createHealthBar(barSetting, scene);
	}

	~ZombieController() {
		if (hpBar) { delete hpBar; }
	}

	void update(GameObject* gameObject, Scene* scene) override {
		Zombie* zombie = (Zombie*) gameObject;
		rootNode->loadGameObject(zombie);
		//modelNode->switchAnim(zombie->animation->animationType);
	}

	void processZombieDeath() {
	}
};