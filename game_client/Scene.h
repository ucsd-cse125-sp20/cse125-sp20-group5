#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <glm\gtx\euler_angles.hpp>

#include <GameState.hpp>
#include "Model.h"
#include "Ground.h"
#include "AssimpModel.h"
#include "AnimatedAssimpModel.h"
#include "Constants.h"
#include "Shader.h"
#include "ParticleFactory.hpp"

class Scene
{
private:
	std::vector<Model*> models;
	Ground * ground;

	GameState * state;

	// scene gprah values
	SceneNode* rootNode;
	SceneNode* groundNode;
	std::unordered_map<uint, SceneNode*> objectIdMap;

	// models
	AnimatedAssimpModel * zombieModel;
	AnimatedAssimpModel* playerModel;
	AssimpModel* tapModel;
	AssimpModel* toolModel;

	// shaders
	ShaderProgram * program;
	ShaderProgram * assimpProgram;
	ShaderProgram* animationProgram;

	// this is a temp thing until we get animation from server;
	chrono::system_clock::time_point startTime;

	SceneNode* getDrawableSceneNode(uint objectId, Drawable * model);

	// Particle effect
	ParticleFactory* particleFactory;
	ParticleGroup* waterTapParticles;
	ShaderProgram* particleProgram;

public:
	Scene();
	~Scene();

	void setState(GameState * state);
	void update(); // recalcuate the matrices
	void draw(const glm::mat4 &veiwProjMat); // renders everything

	// static function for a to create a specfic scene good for grahics testing
	static Scene* testScene();
	static Scene* scene0();
};

