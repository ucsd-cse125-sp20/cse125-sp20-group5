#include "Scene.h"

Scene::Scene()
{
	program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	assimpProgram = new ShaderProgram("AssimpModel.glsl", ShaderProgram::eRender);
	animationProgram = new ShaderProgram("AnimatedAssimpModel.glsl", ShaderProgram::eRender);
	
	zombieModel = new AnimatedAssimpModel(ZOMBIE_MODEL, animationProgram->GetProgramID());
	playerModel = new AnimatedAssimpModel(PLAYER_MODEL, animationProgram->GetProgramID());
	tapModel = new AssimpModel(WATER_TAP_MODEL, assimpProgram->GetProgramID());

	ground = NULL;

	rootNode = new SceneNode(NULL, string("absoluteRoot"), 0);

	startTime = chrono::system_clock::now();
}

Scene::~Scene()
{
	for (Model* mod : models) {
		delete mod;
	}
	delete ground;

	delete zombieModel;
	delete playerModel;

	delete program;
	delete assimpProgram;
	delete animationProgram;
	delete rootNode;
}

glm::mat4 getPoseFromDirection(float angle) {
	return glm::eulerAngleZ(angle);
}

void Scene::update()
{
	// Clear the two vectors
	zombieTransfroms.clear();
	playerTransforms.clear();

	if (ground == NULL) {
		ground = new Ground(state->tiles.size(), state->tiles[0].size(), 1.0, 10, 10, program->GetProgramID());
	}

	for (int i = 0; i < state->tiles.size(); i++) {
		for (int j = 0; j < state->tiles[0].size(); j++) {
			ground->setLoc(i,j,(Ground::TILE_TYPE)(state->tiles[i][j]->tileType));
		}
	}

	ground->update(NULL);

	for (Zombie* zombie : state->zombies) {
		Position* position = zombie->position;
		Direction* direction = zombie->direction;

		SceneNode* zombieTemp = zombieModel->createSceneNodes(1, NULL);

		chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - startTime;
		float runningTime = elapsed_seconds.count();
		zombieTemp->animationTime = runningTime;

		zombieTemp->position[0] = position->getX();
		zombieTemp->position[1] = position->getY();
		zombieTemp->position[2] = position->getZ();

		zombieTemp->dir = direction->angle;

		zombieTemp->update(glm::mat4(1.0));

		zombieTransfroms.push_back(zombieTemp);
	}

	for (Player* player : state->players) {
		Position* position = player->position;
		Direction* direction = player->direction;

		SceneNode * playerTemp = playerModel->createSceneNodes(player->playerId, NULL);

		chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - startTime;
		float runningTime = elapsed_seconds.count();
		playerTemp->animationTime = runningTime;

		playerTemp->position[0] = position->getX();
		playerTemp->position[1] = position->getY();
		playerTemp->position[2] = position->getZ();

		playerTemp->dir = direction->angle;

		playerTemp->update(glm::mat4(1.0));
		playerTransforms.push_back(playerTemp);
	}

	tapTransform = glm::mat4(1.0);
	tapTransform[3][0] = state->waterTap->position->getX();
	tapTransform[3][1] = state->waterTap->position->getY();
	tapTransform[3][2] = state->waterTap->position->getZ();

	tapTransform = tapTransform * getPoseFromDirection(state->waterTap->direction->angle) * WATER_TAP_SCALER;

	for (Model* model : models) {
		model->update(NULL);
	}
}

void Scene::draw(const glm::mat4 &veiwProjMat)
{
	ground->draw(NULL, glm::mat4(1.0), veiwProjMat);

	for (SceneNode * node : zombieTransfroms) {
		// Add transform to assimp models
		node->draw(veiwProjMat);
	}

	for (SceneNode * node : playerTransforms) {
		// Add transform to assimp models
		node->draw(veiwProjMat);
	}

	tapModel->draw(NULL, tapTransform, veiwProjMat);

	for (Model * model : models) {
		model->draw(NULL, model->getLocalMat(), veiwProjMat);
	}	
}


// Update the current gamestate
void Scene::setState(GameState* state) 
{
	this->state = state;
}



// static function for a to create a specfic scene I imagine one of these for each level/screen

// currently this is a test set up just testing drawing and input
Scene * Scene::testScene() {
	Scene * scene = new Scene;

	Model * cube = new Model(scene->program->GetProgramID());
	cube->makeBox(glm::vec3(-.25,0,-.25), glm::vec3(.25,.5,.25));

	Model* floor = new Model(scene->program->GetProgramID());
	floor->makeBox(glm::vec3(-2, -0.05, -2), glm::vec3(2, 0, 2));

	Model* tile = new Model(scene->program->GetProgramID());
	tile->makeTile(glm::vec3(0, -3, 0), glm::vec3(.1, -3, .1), glm::vec3(.8, .3, .12), "grass1tile.png");

	scene->models.push_back(cube);
	scene->models.push_back(floor);
	scene->models.push_back(tile);
	return scene;
}

Scene* Scene::scene0() {
	Scene* scene = new Scene;

	Model* cube = new Model(scene->program->GetProgramID());
	cube->makeBox(glm::vec3(-.25, 0, -.25), glm::vec3(.25, .5, .25));

	scene->ground = Ground::ground0(scene->program->GetProgramID());

	scene->models.push_back(cube);

	return scene;
}

