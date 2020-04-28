#include "Scene.h"

Scene::Scene()
{
	program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	assimpProgram = new ShaderProgram("AssimpModel.glsl", ShaderProgram::eRender);
	animationProgram = new ShaderProgram("AnimatedAssimpModel.glsl", ShaderProgram::eRender);
	
	zombieModel = new AnimatedAssimpModel(ZOMBIE_MODEL, animationProgram->GetProgramID());
	playerModel = new AnimatedAssimpModel(PLAYER_MODEL, animationProgram->GetProgramID());
	tapModel = new AssimpModel(WATER_TAP_MODEL, assimpProgram->GetProgramID());

	zombieModel->setModelFixer(RABBIT_SCALER);
	playerModel->setModelFixer(PLAYER_SCALER);
	tapModel->setModelFixer(PLAYER_SCALER);

	ground = NULL;

	rootNode = new SceneNode(NULL, string("absoluteRoot"), 0);
	groundNode = new SceneNode(ground, string("ground"), 1);
	rootNode->addChild(groundNode);

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
	// TODO refactor ground in gamestate and to simplify this
	if (ground == NULL) {
		ground = new Ground(state->tiles.size(), state->tiles[0].size(), 1.0, 10, 10, program->GetProgramID());
		groundNode->obj = ground;
	}
	for (int i = 0; i < state->tiles.size(); i++) {
		for (int j = 0; j < state->tiles[0].size(); j++) {
			ground->setLoc(i,j,(Ground::TILE_TYPE)(state->tiles[i][j]->tileType));
		}
	}

	float offestTime = 0;
	for (Zombie* zombie : state->zombies) {

		SceneNode* zombieTemp = getDrawableSceneNode(zombie->objectId, zombieModel);
		zombieTemp->loadGameObject(zombie); // load new data

		// this is only here becuase there server sint sending it right now
		chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - startTime;
		float runningTime = elapsed_seconds.count();
		zombieTemp->animationTime = runningTime + offestTime;
		offestTime += 4.0;
	}


	for (Player* player : state->players) {
		
		SceneNode * playerTemp = getDrawableSceneNode(player->objectId, playerModel);
		playerTemp->loadGameObject(player);

		// here is wehre we handle stuff like making sure they are holding another object
		if (player->holding) {
			if (objectIdMap.count(player->heldObject) > 0) {
				SceneNode * heldNode = objectIdMap[player->heldObject];
				SceneNode * playerHand = playerTemp->find(std::string("j_r_arm_$AssimpFbx$_Translation"), playerTemp->objectId); 
				if (heldNode != NULL && playerHand != NULL) {
					if (heldNode->parent != playerHand) {
						playerHand->addChild(heldNode);
						// TODO the matrix will have to be a constant we need to figure out how to make it look held
						heldNode->transform = glm::mat4(1.0);
					}
				}
			}
		}

		// this is only here becuase there server isnt sending it right now
		chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - startTime;
		float runningTime = elapsed_seconds.count();
		playerTemp->animationTime = runningTime+offestTime;
		offestTime += 0.3;
	}

	SceneNode* tapNode = getDrawableSceneNode(state->waterTap->objectId,tapModel);
	tapNode->loadGameObject(state->waterTap);

	for (Tool * tool : state->tools) {
		SceneNode* toolTemp = getDrawableSceneNode(tool->objectId, tapModel);
		toolTemp->loadGameObject(tool); // load new data
	}
	
	rootNode->update(glm::mat4(1.0));

	// this is test sode remove it at some point;
	for (Model* model : models) {
		model->update(NULL);
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
		groundNode->addChild(node); // this should be the ground or maybe a parameter
	}
	else { // if its made just get the ref
		node = objectIdMap[objectId];
	}
	return node;
}

void Scene::draw(const glm::mat4 &veiwProjMat)
{

	rootNode->draw(veiwProjMat);

	// this is for testing we should be bale to remove at some point
	SceneNode temp(NULL, std::string(""), 0);
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

