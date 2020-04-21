#pragma once
#include <vector>
#include <unordered_map>
#include "Model.h"
#include "SpinningCube.h"
#include "Ground.h"
#include <GameState.hpp>
#include "AssimpModel.h"

class Scene
{
private:
	std::vector<Model*> models;
	Ground * ground;

	GameState * state;

	std::vector<glm::mat4> zombieTransfroms;
	std::vector<glm::mat4> playerTransforms;

	AssimpModel * zombieModel;
	AssimpModel * playerModel; 
public:
	Scene();
	~Scene();

	void setState(GameState * state);
	void update(); // recalcuate the matrices
	void draw(const glm::mat4 &veiwProjMat, uint shader); // renders everything

	// static function for a to create a specfic scene I imagine one of these for each level/screen
	static Scene* testScene();
	static Scene* scene0();
};

