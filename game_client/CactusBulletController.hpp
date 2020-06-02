#include "Core.h"
#include "Scene.h"
#include "RenderController.hpp"

#define BULLET_SCALER 0.2
#define BULLET_HEIGHT .8

class CactusBulletController : public RenderController {
private:

public:
	CactusBulletController(CactusBullet* bullet, Scene* scene) {
		rootNode = new SceneNode(NULL, "RootCactusBulletEmpty", bullet->objectId);
		modelNode = scene->getModel(ModelType::CACTUS_BULLET)->createSceneNodes(bullet->objectId);
		rootNode->addChild(modelNode);
		scene->getGroundNode()->addChild(rootNode);
		rootNode->scaler = BULLET_SCALER;
	}

	void update(GameObject* gameObject, Scene* scene) override {
		CactusBullet* bullet = (CactusBullet*)gameObject;
		rootNode->loadGameObject(bullet);
		rootNode->position[1] = BULLET_HEIGHT;
	}
};