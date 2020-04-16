#pragma once
#include <vector>
#include <unordered_map>
#include "Model.h"
#include "SpinningCube.h"
#include "Ground.h"

class Scene
{
private:
	std::vector<Model*> models;
	Ground * ground;
public:
	Scene();
	~Scene();
	void getState(std::unordered_map<int, bool> * keyPresses); // get a new state
	void update(); // recalcuate the matrices
	void draw(const glm::mat4 &veiwProjMat, uint shader); // renders everything

	// static function for a to create a specfic scene I imagine one of these for each level/screen
	static Scene* testScene();
	static Scene* scene0();
};

