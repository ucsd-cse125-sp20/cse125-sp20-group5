#include "Scene.h"
#include <iostream>

Scene::Scene()
{
	
}

Scene::~Scene()
{
	for (Model* mod : models) {
		delete mod;
	}
	delete ground;
}

// this is a dummy fucniton it should take the netowrk values and make sense of them
void Scene::getState(std::unordered_map<int,bool> * keyPresses)
{
	float stepSize = 0.05;
	if ((*keyPresses)[GLFW_KEY_W]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(0, 0, -stepSize));
	}
	if ((*keyPresses)[GLFW_KEY_A]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(-stepSize, 0, 0));
	}
	if ((*keyPresses)[GLFW_KEY_S]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(0, 0, stepSize));
	}
	if ((*keyPresses)[GLFW_KEY_D]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(stepSize, 0, 0));
	}
}

void Scene::update()
{
	for (Model* model : models) {
		model->update();
	}
	ground->update();
}

void Scene::draw(const glm::mat4 &veiwProjMat, uint shader)
{
	for (Model * model : models) {
		model->draw(model->getLocalMat(), veiwProjMat, shader);
	}
	ground->draw(veiwProjMat, shader);
}




// static function for a to create a specfic scene I imagine one of these for each level/screen

// currently this is a test set up just testing drawing and input
Scene * Scene::testScene() {
	Scene * scene = new Scene;

	Model * cube = new Model;
	cube->makeBox(glm::vec3(-.25,0,-.25), glm::vec3(.25,.5,.25));

	Model* floor = new Model;
	floor->makeBox(glm::vec3(-2, -0.05, -2), glm::vec3(2, 0, 2));

	Model* tile = new Model;
	tile->makeTile(glm::vec3(0, -3, 0), glm::vec3(.1, -3, .1), glm::vec3(.8, .3, .12), "grass1tile.png");

	scene->models.push_back(cube);
	scene->models.push_back(floor);
	scene->models.push_back(tile);
	return scene;
}

Scene* Scene::scene0() {
	Scene* scene = new Scene;

	Model* cube = new Model;
	cube->makeBox(glm::vec3(-.25, 0, -.25), glm::vec3(.25, .5, .25));

	scene->ground = Ground::ground0();

	scene->models.push_back(cube);

	return scene;
}

