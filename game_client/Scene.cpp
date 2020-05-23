#include "Scene.h"
#include "RenderController.hpp"
#include "PlantController.hpp"
#include "TapController.hpp"
#include "ToolController.hpp"
#include "PlayerController.hpp"
#include "ZombieController.hpp"
#include "BaseController.hpp"

Scene::Scene()
{
	program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	assimpProgram = new ShaderProgram("AssimpModel.glsl", ShaderProgram::eRender);
	animationProgram = new ShaderProgram("AnimatedAssimpModel.glsl", ShaderProgram::eRender);
	skyboxProgram = new ShaderProgram("Skybox.glsl", ShaderProgram::eRender);
	uiProgram = new ShaderProgram("UI.glsl", ShaderProgram::eRender);
	barProgram = new ShaderProgram("HealthBar.glsl", ShaderProgram::eRender);
	
	zombieModel = new AnimatedAssimpModel(ZOMBIE_MODEL, animationProgram->GetProgramID());
	playerModel = new AnimatedAssimpModel(PLAYER_MODEL, animationProgram->GetProgramID());
	seedModel = new AssimpModel(SEED_MODEL, assimpProgram->GetProgramID());
	saplingModel = new AssimpModel(SAPLING_MODEL, assimpProgram->GetProgramID());
	babyCornModel = new AssimpModel(BABY_CORN_MODEL, assimpProgram->GetProgramID());	
	cornModel = new AssimpModel(CORN_MODEL, assimpProgram->GetProgramID());
	tapModel = new AssimpModel(WATER_TAP_MODEL, assimpProgram->GetProgramID());
	wateringCanModel = new AssimpModel(WATERING_CAN_MODEL, assimpProgram->GetProgramID());
	seedSourceModel = new AssimpModel(SEED_SOURCE_MODEL, assimpProgram->GetProgramID());
	shovelModel = new AssimpModel(SHOVEL_MODEL, assimpProgram->GetProgramID());
	seedBagModel = new AssimpModel(SEED_BAG_MODEL, assimpProgram->GetProgramID());
	baseModel = new AssimpModel(HOME_BASE_MODEL, assimpProgram->GetProgramID());

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
	delete playerModel;

	delete seedModel;
	delete saplingModel;
	delete babyCornModel;
	delete cornModel;
	delete tapModel;
	delete wateringCanModel;
	delete seedSourceModel;
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

	for (Zombie* zombie : state->zombies) {
		if (controllers.find(zombie->objectId) == controllers.end()) {
			controllers[zombie->objectId] = new ZombieController(zombie, this);
			objectIdMap[zombie->objectId] = controllers[zombie->objectId]->rootNode;
		}
		controllers[zombie->objectId]->update(zombie, this);
	}
	ZombieController::processZombieDeath(this);

	
	HomeBase* homeBase = state->homeBase;
	if (controllers.find(homeBase->objectId) == controllers.end()) {
		controllers[homeBase->objectId] = new BaseController(homeBase, this);
		objectIdMap[homeBase->objectId] = controllers[homeBase->objectId]->rootNode;
	}
	controllers[homeBase->objectId]->update(homeBase, this);
	unusedIds.erase(homeBase->objectId);
	
	for (Plant* plant : state->plants) {
		if (controllers.find(plant->objectId) == controllers.end()) {
			controllers[plant->objectId] = new PlantController(plant, this);
			objectIdMap[plant->objectId] = controllers[plant->objectId]->rootNode;
		}
		controllers[plant->objectId]->update(plant, this);

		unusedIds.erase(plant->objectId);
	}

	for (Tool* tool : state->tools) {
		if (controllers.find(tool->objectId) == controllers.end()) {
			controllers[tool->objectId] = new ToolController(tool, this);
			objectIdMap[tool->objectId] = controllers[tool->objectId]->rootNode;
		}
		controllers[tool->objectId]->update(tool, this);

		unusedIds.erase(tool->objectId);
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

	SceneNode* seedShackNode = getDrawableSceneNode(state->seedShack->objectId, seedSourceModel);
	seedShackNode->loadGameObject(state->seedShack);
	seedShackNode->scaler = SEED_SOURCE_SCALER;
	seedShackNode->position[1] = .65; // TODO MAKE THIS A CONSTANT WHEN THE SIZES ARE SET
	unusedIds.erase(state->seedShack->objectId);
	
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