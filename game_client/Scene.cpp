#include "Scene.h"
#include <set>

Scene::Scene()
{
	program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	assimpProgram = new ShaderProgram("AssimpModel.glsl", ShaderProgram::eRender);
	animationProgram = new ShaderProgram("AnimatedAssimpModel.glsl", ShaderProgram::eRender);
	skyboxProgram = new ShaderProgram("Skybox.glsl", ShaderProgram::eRender);
	uiProgram = new ShaderProgram("UI.glsl", ShaderProgram::eRender);
	
	zombieModel = new AnimatedAssimpModel(ZOMBIE_MODEL, animationProgram->GetProgramID());
	playerModel = new AnimatedAssimpModel(PLAYER_MODEL, animationProgram->GetProgramID());
	cornModel = new AssimpModel(CORN_MODEL, assimpProgram->GetProgramID());
	tapModel = new AssimpModel(WATER_TAP_MODEL, assimpProgram->GetProgramID());
	wateringCanModel = new AssimpModel(WATERING_CAN_MODEL, assimpProgram->GetProgramID());
	seedSourceModel = new AssimpModel(SEED_SOURCE_MODEL, assimpProgram->GetProgramID());
	shovelModel = new AssimpModel(SHOVEL_MODEL, assimpProgram->GetProgramID());

	ground = NULL;

	rootNode = new SceneNode(NULL, string("absoluteRoot"), 0);
	groundNode = new SceneNode(ground, string("ground"), 1);
	rootNode->addChild(groundNode);

	skybox = new Skybox(skyboxProgram->GetProgramID(), glm::scale(glm::vec3(100.0f)));

	testUI = new Image2d(uiProgram->GetProgramID(), "texture/newheart.ppm", 0.1, glm::vec2((1.6 * 0 + 0.8) * 0.1 - 1.0, 0.12 - 1.0), 2, 0.9);  //TODO to be removed

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
		unusedIds.insert(kvp.first);
	}

	// TODO refactor ground in gamestate and to simplify this
	Floor* floor = state->floor;
	if (ground == NULL) {
		ground = new Ground(floor->tiles[0].size(), floor->tiles.size(), 1.0, 10, 10, program->GetProgramID());
		groundNode->obj = ground;
		groundNode->position = glm::vec3(floor->tiles.size()/(-1.5), 0, floor->tiles[0].size()/(-2.0));
	}
	for (int i = 0; i < floor->tiles.size(); i++) {
		for (int j = 0; j < floor->tiles[0].size(); j++) {
			ground->setLoc(j,i,(Ground::TILE_TYPE)(floor->tiles[i][j]->tileType));
		}
	}


	for (Zombie* zombie : state->zombies) {
		SceneNode* zombieNode = getDrawableSceneNode(zombie->objectId, zombieModel);
		zombieNode->loadGameObject(zombie); // load new data
		zombieNode->scaler = RABBIT_SCALER; // i dont' love this set up though its not the worst
		unusedIds.erase(zombie->objectId);  // perhaps the server could provide it
	}

	for (Plant* plant : state->plants) {
		SceneNode* plantNode = getDrawableSceneNode(plant->objectId, cornModel);
		plantNode->loadGameObject(plant); // load new data
		plantNode->scaler = RABBIT_SCALER; // i dont' love this set up though its not the worst
		plantNode->position[1] = .7;
		unusedIds.erase(plant->objectId);  // perhaps the server could provide it
	}

	for (Player* player : state->players) {
		SceneNode* playerNode = getDrawableSceneNode(player->objectId, playerModel);
		playerNode->loadGameObject(player);
		playerNode->scaler = PLAYER_SCALER;
		unusedIds.erase(player->objectId);

		// here is wehre we handle stuff like making sure they are holding another object
		if (player->holding) {
			if (objectIdMap.count(player->heldObject) > 0) {
				SceneNode * heldNode = objectIdMap[player->heldObject];
				SceneNode * playerHand = playerNode->find(std::string("j_r_arm_$AssimpFbx$_Translation"), playerNode->objectId);
				if (heldNode != NULL && playerHand != NULL) {
					if (heldNode->parent != playerHand) {
						playerHand->addChild(heldNode);
						// TODO the values will have to be a constant we need to figure out how to make it look held
						if (heldNode->obj == wateringCanModel) {
							heldNode->scaler = WATER_CAN_SCALER / PLAYER_SCALER;
							heldNode->position = WATER_CAN_HOLD_VEC;
						}
						else if (heldNode->obj == shovelModel) {
							heldNode->scaler = SHOVEL_SCALER / PLAYER_SCALER;
							heldNode->position = SHOVEL_HOLD_VEC;
						}
					}
				}
			}
		}
	}

	SceneNode* tapNode = getDrawableSceneNode(state->waterTap->objectId,tapModel);
	if (tapNode->countChildern() == 0) {
		ParticleGroup* pGroup = particleFactory->getWaterTapParticleGroup(glm::vec3(0, 0, 0));
		SceneNode* waterNode = pGroup->createSceneNodes(state->waterTap->objectId);
		waterNode->position = glm::vec3(0,6.0,-2.0);
		tapNode->addChild(waterNode);
	}
	tapNode->loadGameObject(state->waterTap);
	tapNode->scaler = WATER_TAP_SCALER;
	unusedIds.erase(state->waterTap->objectId);

	for (Tool * tool : state->tools) {
		SceneNode* toolNode;
		float toolScaler = 1.0;
		if (tool->toolType == Tool::ToolType::WATER_CAN) { // TODO make this a constant
			toolNode = getDrawableSceneNode(tool->objectId, wateringCanModel);
			toolScaler = WATER_CAN_SCALER;
		} 
		else {
			toolNode = getDrawableSceneNode(tool->objectId, shovelModel);
			toolScaler = SHOVEL_SCALER;
		}

		if (!tool->held) {
			if (toolNode->parent != groundNode) {
				toolNode->setParent(groundNode);
			}
			toolNode->loadGameObject(tool); // load new data
			toolNode->scaler = toolScaler;
		}
		unusedIds.erase(tool->objectId);
	}

	SceneNode* seedShackNode = getDrawableSceneNode(state->seedShack->objectId, seedSourceModel);
	seedShackNode->loadGameObject(state->seedShack);
	seedShackNode->scaler = SEED_SOURCE_SCALER;
	seedShackNode->position[1] = .65; // TODO MAKE THIS A CONSTANT WHEN THE SIZES ARE SET
	unusedIds.erase(state->seedShack->objectId);
	
	rootNode->update(glm::mat4(1.0));

	// TODO WARNING this is not safe we need code hanlding palyyare disappearing
	// while holding stuff. right now that will cuase an ERROR
	for (uint id : unusedIds) {
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
		node->scaler = .5;
		groundNode->addChild(node); // this should be the ground or maybe a parameter
	}
	else { // if its made just get the ref
		node = objectIdMap[objectId];
	}
	return node;
}

void Scene::draw(const glm::mat4 &viewProjMat)
{
	skybox->draw(viewProjMat);

	rootNode->draw(viewProjMat);

	testUI->draw(); //TODO to be removed
}

void Scene::toggleWater()
{
	SceneNode* tapNode = getDrawableSceneNode(state->waterTap->objectId, tapModel);
	((ParticleGroup*)(tapNode->children.begin()->second->obj))->toggleSpawning();

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