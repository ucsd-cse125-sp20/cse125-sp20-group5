#pragma once
#include "RenderController.hpp"
#include <WaterTap.hpp>
#include "Scene.h"

#define WATER_TAP_SCALER 0.3
#define PARTICLE_POSITION glm::vec3(0,6.0,-2.0)

class TapController : public RenderController {
public:
	TapController(uint objectId, Scene* scene) {
		rootNode = new SceneNode(NULL, "TapRootEmpty", objectId);
		modelNode = scene->getModel(ModelType::WATER_TAP)->createSceneNodes(objectId);
		modelNode->scaler = WATER_TAP_SCALER;
		rootNode->addChild(modelNode);
		scene->getGroundNode()->addChild(rootNode);

		pGroup = scene->getParticleFactory()->getWaterTapParticleGroup(glm::vec3(0));
		particleNode = pGroup->createSceneNodes(objectId);
		particleNode->position = PARTICLE_POSITION;
		modelNode->numAnimation = 2;
		modelNode->addChild(particleNode);
		toggleWater();
	}

	void update(GameObject* gameObject, Scene* scene) override {
		WaterTap* tap = (WaterTap*)gameObject;

		rootNode->loadGameObject(tap);
		if (modelNode->switchAnim(tap->animation->animationType)) {
			toggleWater();
		}
		
		// we should handle turn on and off the water tap here
	}

	void toggleWater() {
		pGroup->toggleSpawning();
	}

	~TapController() {
		delete pGroup;
	}

private:
	ParticleGroup* pGroup;
	SceneNode* particleNode;
};