#pragma once
#include "Scene.h"
#include "RenderController.h"

#define RABBIT_SCALER 0.40

class ZombieController : public RenderController {
public:
	ZombieController(Zombie* zombie, Scene* scene) {
		rootNode = new SceneNode(NULL, "ZombieRootEmpty" + zombie->objectId, zombie->objectId);
		modelNode = scene->getModel(ModelType::RABBIT)->createSceneNodes(zombie->objectId);
		modelNode->loopAnimation = false;
		modelNode->scaler = RABBIT_SCALER;

		rootNode->addChild(modelNode);
		scene->getGroundNode()->addChild(rootNode);
	}

	void update(GameObject* gameObject, Scene* scene) override {
		update((Zombie*)gameObject, scene);
	}

	void update(Zombie* player, Scene* scene) {
		rootNode->loadGameObject(player);
		modelNode->switchAnim(player->animation->animationType);
	}
};