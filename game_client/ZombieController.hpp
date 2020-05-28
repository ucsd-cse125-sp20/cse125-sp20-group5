#pragma once
#include "Scene.h"
#include "RenderController.hpp"

class ZombieController : public RenderController {
private:
	HealthBar* hpBar;
	SceneNode* barNode;

	std::chrono::system_clock::time_point lastBarUpdateTime; // to allow the hpBar display for awhile
	static constexpr int BAR_RENDER_MILLISEC = 1000;

	int health = 0;
	int maxHealth = 0;

	static constexpr float RABBIT_SCALER = 0.40;
	static constexpr float HP_BAR_TRANSLATE_Y = 2.7;
	static constexpr glm::vec3 HP_BAR_COLOR = glm::vec3(1.0, 0.4, 0.4);

public:
	ZombieController(Zombie* zombie, Scene* scene) {
		rootNode = new SceneNode(NULL, "ZombieRootEmpty" + to_string(zombie->objectId), zombie->objectId);
		modelNode = scene->getModel(ModelType::RABBIT)->createSceneNodes(zombie->objectId);
		modelNode->scaler = RABBIT_SCALER;

		rootNode->addChild(modelNode);
		scene->getGroundNode()->addChild(rootNode);

		// init hp bar
		float initBarFilledFraction = 1.0f;
		HealthBarSetting barSetting(
			"texture/hp_icon.png", HP_BAR_TRANSLATE_Y, initBarFilledFraction, HP_BAR_COLOR
		);
		std::tie(hpBar, barNode) = createHealthBar(barSetting, scene);

		// hp bar rendering settings
		hpBar->fillingStep *= 0.2f;
		hpBar->alphaEffectOn = true;

		lastBarUpdateTime = std::chrono::system_clock::now() - std::chrono::milliseconds(BAR_RENDER_MILLISEC);

		// add to renderedZombie to keep track of alive/dead zombie
		prevAliveZombie.insert(zombie->objectId);
	}

	~ZombieController() {
		if (barNode) {
			barNode = RenderController::deleteBarNode(barNode);
		}
		if (hpBar) { delete hpBar; }
	}

	void update(GameObject* gameObject, Scene* scene) override {
		Zombie* zombie = (Zombie*) gameObject;
		rootNode->loadGameObject(zombie);
		
		// animation
		int newAnimID = zombie->animation->animationType;
		if (newAnimID == Zombie::DAMAGED) {
			modelNode->switchAnim(newAnimID, false);
		} 
		// assuming server will only pass in MOVE & DAMAGED animID
		// change back to MOVE if DAMAGED has been finished playing
		else if (modelNode->animationId == Zombie::DAMAGED 
			&& modelNode->playedOneAnimCycle) {
			modelNode->switchAnim(newAnimID);
		}

		this->health = zombie->health;
		this->maxHealth = zombie->maxHealth;

		updateHpBar(scene);

		aliveZombie.insert(zombie->objectId);
	}

	static std::set<uint> prevAliveZombie;
	static std::set<uint> aliveZombie;
	static std::set<uint> dyingZombie;

	// should be called right after updating all zombies
	static void processZombieDeath(Scene* scene) {
		// Get new dying zombie
		if (prevAliveZombie.size() > aliveZombie.size()) {
			// Server has killed some zombies (;o;)
			std::set_difference(
				prevAliveZombie.begin(), prevAliveZombie.end(),
				aliveZombie.begin(), aliveZombie.end(),
				std::inserter(dyingZombie, dyingZombie.end())
			);
			prevAliveZombie = aliveZombie;
		}

		// Handle dying logic
		for (auto it = dyingZombie.begin(); it != dyingZombie.end();) {
			uint zombieId = *it;
			ZombieController* zombieController = (ZombieController*) scene->controllers[zombieId];


			// If not disppearing at the home base, but killed by the plant
			// which is TODO
			if (true) { // zombieController->health <= 0
				zombieController->modelNode->switchAnim(Zombie::ZombieAnimation::DIE, false);

				// Don't delete if animation hasn't finished
				if (!zombieController->modelNode->playedOneAnimCycle) {
					// Still update the bar while dying
					zombieController->health = 0;
					zombieController->updateHpBar(scene);
					it++;
					continue;
				}
			}

			// Delete if reached to the home base, or killed and dying animation has finished

			// Delete the bar if is going to die
			// Might be redundant with the destructor but has to be called here or the program crashes somehow
			zombieController->barNode = RenderController::deleteBarNode(zombieController->barNode);

			delete scene->controllers[zombieId];
			scene->controllers.erase(zombieId);
			delete scene->objectIdMap[zombieId];
			scene->objectIdMap.erase(zombieId);
			it = dyingZombie.erase(it);
		}

		// reset aliveZombie for next update cycle
		aliveZombie.clear(); // or = std::set<uintd>();     
	}

	void updateHpBar(Scene* scene) {
		if (this->maxHealth <= 0) {
			return; // data hasn't been received from server yet
		}

		if (hpBar->currFilledFraction <= 0.0) {
			if (barNode) {
				barNode = RenderController::deleteBarNode(barNode);
			}
		}
		else {
			float newFilledFraction = (float)this->health / (float)this->maxHealth;
			if (hpBar->currFilledFraction == newFilledFraction) {
				// render bar for a while after bar stops changing
				if (hpBar->shouldDisplay) {
					auto durationMilliSec = std::chrono::duration_cast<std::chrono::milliseconds > (std::chrono::system_clock::now() - lastBarUpdateTime);
					if (durationMilliSec.count() > BAR_RENDER_MILLISEC) {
						hpBar->shouldDisplay = false;
					}
				}
			}
			else {
				hpBar->shouldDisplay = true; // display only when the bar is changing
				hpBar->updateBar(newFilledFraction);

				lastBarUpdateTime = std::chrono::system_clock::now();
			}
		}
	}
};

std::set<uint> ZombieController::prevAliveZombie;
std::set<uint> ZombieController::aliveZombie;
std::set<uint> ZombieController::dyingZombie;