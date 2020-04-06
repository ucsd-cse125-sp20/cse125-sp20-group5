#include "Scene.h"
#include <iostream>

Scene::Scene()
{
	
}

// this is a dummy fucniton it should take the netowrk values and make sense of them
void Scene::getState(std::unordered_map<int,bool> * keyPresses)
{
	if ((*keyPresses)[GLFW_KEY_W]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(0, 0, -.01));
	}
	if ((*keyPresses)[GLFW_KEY_A]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(-.01, 0, 0));
	}
	if ((*keyPresses)[GLFW_KEY_S]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(0, 0,.01));
	}
	if ((*keyPresses)[GLFW_KEY_D]) {
		models[0]->setPosition(models[0]->getPosition() + glm::vec3(0.01, 0, 0));
	}
}

void Scene::update()
{
	for (Model* model : models) {
		model->update();
	}
}

void Scene::draw(const glm::mat4 &veiwProjMat, uint shader)
{
	for (Model * model : models) {
		model->draw(model->getLocalMat(), veiwProjMat, shader);
	}
}


// static function for a to create a specfic scene I imagine one of these for each level/screen
// currently this is a test set up
Scene * Scene::testScene() {
	Scene * scene = new Scene;

	Model * cube = new Model;
	cube->makeBox(glm::vec3(-.25,0,-.25), glm::vec3(.25,.5,.25));

	Model* floor = new Model;
	floor->makeBox(glm::vec3(-2, -0.05, -2), glm::vec3(2, 0, 2));

	scene->models.push_back(cube);
	scene->models.push_back(floor);
	return scene;
}

