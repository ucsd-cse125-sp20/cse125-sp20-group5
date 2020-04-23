#include "Scene.h"

Scene::Scene()
{
	program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	assimpProgram = new ShaderProgram("AssimpModel.glsl", ShaderProgram::eRender);

	zombieModel = new AssimpModel(ZOMBIE_MODEL, assimpProgram->GetProgramID());
	playerModel = new AssimpModel(PLAYER_MODEL, assimpProgram->GetProgramID());
	ground = NULL;
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
}


void Scene::update()
{
	// Clear the two vectors
	zombieTransfroms.clear();
	playerTransforms.clear();

	if (ground == NULL) {
		ground = new Ground(state->tiles.size(), state->tiles[0].size(), .5, 2, 2, program->GetProgramID());
	}

	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < 1; j++) {
			ground->setLoc(i,j,(Ground::TILE_TYPE)(state->tiles[i][j]->tileType));

		}
	}

	for (Zombie* zombie : state->zombies) {
		Position* position = zombie->position;
		Direction* direction = zombie->direction;

		glm::mat4 mat4Transform(1.0f);

		// Move
		mat4Transform[3][0] = position->getX();
		mat4Transform[3][1] = position->getY();
		mat4Transform[3][2] = position->getZ();
		// Rotate
		mat4Transform = glm::eulerAngleZ(direction->angle) * mat4Transform;

		zombieTransfroms.push_back(mat4Transform);
	}

	for (Player* player : state->players) {
		Position* position = player->position;
		Direction* direction = player->direction;

		glm::mat4 mat4Transform(1.0f);

		// Move
		mat4Transform[3][0] = position->getX();
		mat4Transform[3][1] = position->getY();
		mat4Transform[3][2] = position->getZ();

		// std::cout << position->getX() << ", " << position->getY() << ", " << position->getZ() << "\n";
		
		// Rotate
		mat4Transform = glm::eulerAngleZ(direction->angle) * mat4Transform;

		playerTransforms.push_back(mat4Transform);
	}

	
	for (Model* model : models) {
		model->update();
	}

	ground->update();
}

void Scene::draw(const glm::mat4 &veiwProjMat)
{
	for (glm::mat4 transform : zombieTransfroms) {
		// Add transform to assimp models
		zombieModel->draw(transform, veiwProjMat);
	}

	for (glm::mat4 transform : playerTransforms) {
		// Add transform to assimp models
		playerModel->draw(transform, veiwProjMat);
	}


	for (Model * model : models) {
		model->draw(model->getLocalMat(), veiwProjMat);
	}
	ground->draw(veiwProjMat);
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

