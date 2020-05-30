#include "Scene.h"
#include "RenderController.hpp"
#include "PlantController.hpp"
#include "TapController.hpp"
#include "ToolController.hpp"
#include "PlayerController.hpp"
#include "ZombieController.hpp"
#include "BaseController.hpp"
#include "CactusBulletController.hpp"

Scene::Scene()
{
	program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	assimpProgram = new ShaderProgram("AssimpModel.glsl", ShaderProgram::eRender);
	animationProgram = new ShaderProgram("AnimatedAssimpModel.glsl", ShaderProgram::eRender);
	skyboxProgram = new ShaderProgram("Skybox.glsl", ShaderProgram::eRender);
	uiProgram = new ShaderProgram("UI.glsl", ShaderProgram::eRender);
	barProgram = new ShaderProgram("HealthBar.glsl", ShaderProgram::eRender);
	

	zombieModel = new AnimatedAssimpModel(ZOMBIE_RABBIT_MODEL, animationProgram->GetProgramID());
	zombiePigModel = new AnimatedAssimpModel(ZOMBIE_PIG_MODEL, animationProgram->GetProgramID());

	playerTigerModel = new AnimatedAssimpModel(PLAYER_TIGER_MODEL, animationProgram->GetProgramID());
	playerCatModel = new AnimatedAssimpModel(PLAYER_CAT_MODEL, animationProgram->GetProgramID());
	playerPigModel = new AnimatedAssimpModel(PLAYER_PIG_MODEL, animationProgram->GetProgramID());
	playerChickenModel = new AnimatedAssimpModel(PLAYER_CHICKEN_MODEL, animationProgram->GetProgramID());

	saplingModel = new AnimatedAssimpModel(SAPLING_MODEL, animationProgram->GetProgramID());
	seedModel = saplingModel;
	babyCornModel = new AnimatedAssimpModel(BABY_CORN_MODEL, animationProgram->GetProgramID());	
	cornModel = new AnimatedAssimpModel(CORN_MODEL, animationProgram->GetProgramID());
	babyCactusModel = new AnimatedAssimpModel(BABY_CACTUS_MODEL, animationProgram->GetProgramID());
	cactusModel = new AnimatedAssimpModel(CACTUS_MODEL, animationProgram->GetProgramID());
	cactusBulletModel = new AnimatedAssimpModel(CACTUS_BULLET_MODEL, animationProgram->GetProgramID());

	tapModel = new AssimpModel(WATER_TAP_MODEL, assimpProgram->GetProgramID());
	wateringCanModel = new AssimpModel(WATERING_CAN_MODEL, assimpProgram->GetProgramID());
	seedSourceModel_corn = new AssimpModel(CORN_SEED_SOURCE_MODEL, assimpProgram->GetProgramID());
	seedSourceModel_cactus = new AssimpModel(CACTUS_SEED_SOURCE_MODEL, assimpProgram->GetProgramID());
	shovelModel = new AssimpModel(SHOVEL_MODEL, assimpProgram->GetProgramID());
	seedBagModel = new AssimpModel(SEED_BAG_MODEL, assimpProgram->GetProgramID());
	sprayModel = new AssimpModel(SPRAY_MODEL, assimpProgram->GetProgramID());
	fertilizerModel = new AssimpModel(FERTILIZER_MODEL, assimpProgram->GetProgramID());
	baseModel = new AssimpModel(HOME_BASE_MODEL, assimpProgram->GetProgramID());
	treeModel = new AssimpModel(TREE_MODEL, assimpProgram->GetProgramID());

	ground = NULL;

	rootNode = new SceneNode(NULL, string("absoluteRoot"), 0);
	groundNode = new SceneNode(ground, string("ground"), 1);
	rootNode->addChild(groundNode);

	skybox = new Skybox(skyboxProgram->GetProgramID(), glm::scale(glm::vec3(100.0f)));

	testUI = new Image2d(uiProgram->GetProgramID(), "texture/player_one.png", 0.1, glm::vec2((1.6 * 0 + 0.8) * 0.1 - 1.0, 0.12 - 1.0), 2, 0.9);  //TODO to be removed

	particleProgram = new ShaderProgram("Particle.glsl", ShaderProgram::eRender);
	particleFactory = new ParticleFactory(particleProgram->GetProgramID());

}

Scene::~Scene()
{
	delete ground;
	delete skybox;
	delete testUI; //TODO to be removed

	delete zombieModel;
	delete playerCatModel;
	delete playerTigerModel;
	delete playerChickenModel;
	delete playerPigModel;

	delete seedModel;
	delete saplingModel;
	delete babyCornModel;
	delete cornModel;
	delete cactusModel;
	delete babyCactusModel;
	delete cactusBulletModel;
	delete tapModel;
	delete wateringCanModel;
	delete seedSourceModel_corn;
	delete seedSourceModel_corn;
	delete shovelModel;
	delete seedBagModel;

	delete program;
	delete assimpProgram;
	delete animationProgram;
	delete skyboxProgram;
	delete uiProgram;

	delete rootNode;

	delete particleFactory;
	delete particleProgram;

	delete treeModel;
}

void Scene::update()
{
	std::set<uint> unusedIds;
	for (auto kvp : objectIdMap) {
		if (objectIdMap[kvp.first]->name.find("Zombie") != -1) {
			continue;
		}
		unusedIds.insert(kvp.first);
	}

	// TODO refactor ground in gamestate and to simplify this
	Floor* floor = state->floor;
	if (ground == NULL) {
		ground = new Ground(floor->tiles[0].size(), floor->tiles.size(), 1.0, 10, 10, program->GetProgramID(), assimpProgram->GetProgramID());
		groundNode->obj = ground;
		groundNode->position = glm::vec3(floor->tiles.size()/(-1.5), 0, floor->tiles[0].size()/(-2.0));
	}
	for (int i = 0; i < floor->tiles.size(); i++) {
		for (int j = 0; j < floor->tiles[0].size(); j++) {
			ground->setLoc(j,i,(Ground::TILE_TYPE)(floor->tiles[i][j]->tileType));
		}
	}

	HomeBase* homeBase = state->homeBase;
	if (controllers.find(homeBase->objectId) == controllers.end()) {
		controllers[homeBase->objectId] = new BaseController(homeBase, this);
		objectIdMap[homeBase->objectId] = controllers[homeBase->objectId]->rootNode;
	}
	controllers[homeBase->objectId]->update(homeBase, this);
	ZombieController::updateDestination(homeBase->position->x, homeBase->position->y);
	unusedIds.erase(homeBase->objectId);

	for (Zombie* zombie : state->zombies) {
		if (controllers.find(zombie->objectId) == controllers.end()) {
			controllers[zombie->objectId] = new ZombieController(zombie, this);
			objectIdMap[zombie->objectId] = controllers[zombie->objectId]->rootNode;
		}
		controllers[zombie->objectId]->update(zombie, this);
	}
	ZombieController::processZombieDeath(this);
	
	
	for (Plant* plant : state->plants) {
		if (controllers.find(plant->objectId) == controllers.end()) {
			controllers[plant->objectId] = new PlantController(plant, this);
			objectIdMap[plant->objectId] = controllers[plant->objectId]->rootNode;
		}
		controllers[plant->objectId]->update(plant, this);

		unusedIds.erase(plant->objectId);
	}

	for (CactusBullet* bullet : state->bullets) {
		if (controllers.find(bullet->objectId) == controllers.end()) {
			controllers[bullet->objectId] = new CactusBulletController(bullet, this);
			objectIdMap[bullet->objectId] = controllers[bullet->objectId]->rootNode;
		}
		controllers[bullet->objectId]->update(bullet, this);

		unusedIds.erase(bullet->objectId);
	}

	for (Tool* tool : state->tools) {
		if (controllers.find(tool->objectId) == controllers.end()) {
			controllers[tool->objectId] = new ToolController(tool, this);
			objectIdMap[tool->objectId] = controllers[tool->objectId]->rootNode;
		}
		controllers[tool->objectId]->update(tool, this);

		unusedIds.erase(tool->objectId);
	}

	for (Obstacle* obstacle : state->obstacles) {
		SceneNode* obstacleNode = getDrawableSceneNode(obstacle->objectId, treeModel);
		
		obstacleNode->loadGameObject(obstacle);
		obstacleNode->scaler = TREE_SCALER;
		unusedIds.erase(obstacle->objectId);
	}

	for (Player* player : state->players) {
		if (controllers.find(player->objectId) == controllers.end()) {
			controllers[player->objectId] = new PlayerController(player, this);
			objectIdMap[player->objectId] = controllers[player->objectId]->rootNode;
		}
		controllers[player->objectId]->update(player, this);

		unusedIds.erase(player->objectId);
	}

	if (controllers.find(state->waterTap->objectId) == controllers.end()) {
		controllers[state->waterTap->objectId] = new TapController(state->waterTap->objectId, this);
		objectIdMap[state->waterTap->objectId] = controllers[state->waterTap->objectId]->rootNode;
	}
	controllers[state->waterTap->objectId]->update(state->waterTap, this);
	unusedIds.erase(state->waterTap->objectId);

	for (SeedShack* seedShack : state->seedShacks) {
		SceneNode* seedShackNode = nullptr;

		// Load corresponding model based on plants
		if (seedShack->seedType == Plant::PlantType::CORN)
			seedShackNode = getDrawableSceneNode(seedShack->objectId, seedSourceModel_corn);
		else // (seedShack->seedType == Plant::PlantType::CACTUS)
			seedShackNode = getDrawableSceneNode(seedShack->objectId, seedSourceModel_cactus);
	
		seedShackNode->loadGameObject(seedShack);
        seedShackNode->scaler = SEED_SOURCE_SCALER;
        unusedIds.erase(seedShack->objectId);
	}
	
	rootNode->update(glm::mat4(1.0));

	// TODO WARNING this is not safe we need code hanlding player disappearing
	// while holding stuff. right now that will cuase an ERROR
	for (uint id : unusedIds) {
		if (controllers.find(id) != controllers.end()) { // first delete the controller if it exists
			delete controllers[id];
			controllers.erase(id);
		}
		delete objectIdMap[id];
		objectIdMap.erase(id); 
	}
}

// gets or make sthe sceneNode for a given object id and model;
SceneNode* Scene::getDrawableSceneNode(uint objectId, Drawable * model)
{
	SceneNode* node = NULL;
	// if we haven't made make it
	// TOOD we need to delete models that are no longer used at some point
	if (objectIdMap.count(objectId) < 1) {
		node = model->createSceneNodes(objectId);
		objectIdMap[objectId] = node;
		// node->scaler = .5;
		groundNode->addChild(node); // this should be the ground or maybe a parameter
	}
	else { // if its made just get the ref
		node = objectIdMap[objectId];
		if (node->obj != model) {
			delete node;
			node = model->createSceneNodes(objectId);
			objectIdMap[objectId] = node;
			groundNode->addChild(node);
		}
	}
	return node;
}

void Scene::draw(const glm::mat4 &viewProjMat)
{
	skybox->draw(viewProjMat);

	rootNode->draw(viewProjMat);

	RenderController::drawUI(viewProjMat);
	testUI->draw(); //TODO to be removed
}

void Scene::toggleWater()
{
	((TapController*)controllers[state->waterTap->objectId])->toggleWater();
}

// Update the current gamestate
void Scene::setState(GameState* state) 
{
	this->state = state;
}

// static function for a to create a specfic scene I imagine one of these for each level/screen

Scene* Scene::scene0() {
	Scene* scene = new Scene;

	scene->ground = Ground::ground0(scene->program->GetProgramID());

	return scene;
}

void Scene::setupDirectionalLighting(glm::vec3 eyePosition) {
	glm::vec3 lightColor = glm::vec3(1.0f);
	glm::vec3 lightDirection = glm::normalize(glm::vec3(1, 5, 2));

	vector<uint> shaderIDs;
	shaderIDs.push_back(assimpProgram->GetProgramID());
	shaderIDs.push_back(animationProgram->GetProgramID());
	for (uint shader : shaderIDs) {
		glUseProgram(shader);
		glUniform3fv(glGetUniformLocation(shader, "eyepos"), 1, &eyePosition[0]);
		glUniform3fv(glGetUniformLocation(shader, "lightColor"), 1, &lightColor[0]);
		glUniform3fv(glGetUniformLocation(shader, "lightDirection"), 1, &lightDirection[0]);
	}
	glUseProgram(0);
}