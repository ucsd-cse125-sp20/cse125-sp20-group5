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

void Scene::update()
{

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

		SceneNode* zombieTemp = getAnimatedAssimpSceneNode(zombie->objectId, zombieModel);
		zombieTemp->loadGameObject(zombie); // load new data

		// this is only here becuase there server sint sending it right now
		chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - startTime;
		float runningTime = elapsed_seconds.count();
		zombieTemp->animationTime = runningTime;;

	}

	for (Player* player : state->players) {
		
		SceneNode * playerTemp = getAnimatedAssimpSceneNode(player->objectId, playerModel);
		playerTemp->loadGameObject(player);

		// here is wehre we woudl handle stuff like making sure they are holding another object

		// this is only here becuase there server isnt sending it right now
		chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - startTime;
		float runningTime = elapsed_seconds.count();
		playerTemp->animationTime = runningTime;

	}

	SceneNode* node = NULL;
	// if we haven't made make it
	if (objectIdMap.count(state->waterTap->objectId) < 1) {
		node = new SceneNode(tapModel, "waterTap", state->waterTap->objectId);
		objectIdMap[state->waterTap->objectId] = node;
		rootNode->addChild(node); // this should be the ground or maybe a parameter
	}
	else { // if its made just get the ref
		node = objectIdMap[state->waterTap->objectId];
	}
	node->loadGameObject(state->waterTap);
	//not needed noew but hanlde smoe stuff there 
	

	for (Model* model : models) {
		model->update(NULL);
	}
	rootNode->update(glm::mat4(1.0));
}

// gets or make sthe sceneNode for a given object id and model;
SceneNode* Scene::getAnimatedAssimpSceneNode(uint objectId, AnimatedAssimpModel * model)
{
	SceneNode* node = NULL;
	// if we haven't made make it
	if (objectIdMap.count(objectId) < 1) {
		// TODO createScenNodes Shoudl be part of Drawable
		// TODO then this functino could take a Drawable instead of AnimatedAssimpModel
		node = model->createSceneNodes(objectId, NULL);
		objectIdMap[objectId] = node;
		rootNode->addChild(node); // this should be the ground or maybe a parameter
	}
	else { // if its made just get the ref
		node = objectIdMap[objectId];
	}
	return node;
}

void Scene::draw(const glm::mat4 &veiwProjMat)
{
	SceneNode temp(NULL, std::string(""), 0);
	temp.transform = glm::mat4(1.0);
	ground->draw(temp, veiwProjMat);

	rootNode->draw(veiwProjMat);

	temp.transform = glm::mat4(1.0);
	for (Model * model : models) {
		model->draw(temp, veiwProjMat);
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

