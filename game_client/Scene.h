#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <glm\gtx\euler_angles.hpp>
#include <set>

#include <GameState.hpp>
#include "Model.h"
#include "Ground.h"
#include "AssimpModel.h"
#include "AnimatedAssimpModel.h"
#include "Skybox.h"
#include "Image2d.h" //TODO to be removed
#include "Constants.h"
#include "Shader.h"
#include "ParticleFactory.hpp"

class RenderController; //  put declaration here to sidestep header issues

enum class ShaderType {
	DEFAULT, ASSIMP, ANIMATED, SKYBOX, UI, HEALTH_BAR
};

enum class ModelType {
	CAT, PIG, RABBIT, CORN, WATER_TAP, WATERING_CAN, SEED_SHACK, SHOVEL, SEED_BAG, SEED, SAPLING, BABY_CORN
};

class Scene
{
private:
	Ground * ground;

	GameState * state;

	// scene gprah values
	SceneNode* rootNode;
	SceneNode* groundNode;

	// shaders
	ShaderProgram* program;
	ShaderProgram* assimpProgram;
	ShaderProgram* animationProgram;
	ShaderProgram* skyboxProgram;
	ShaderProgram* uiProgram;
	ShaderProgram* barProgram;

	// models
	AnimatedAssimpModel* zombieModel;
	AnimatedAssimpModel* playerModel;

	AssimpModel* seedModel;
	AssimpModel* saplingModel;
	AssimpModel* babyCornModel;
	AssimpModel* cornModel;
	AssimpModel* tapModel;
	AssimpModel* wateringCanModel;
	AssimpModel* seedSourceModel;
	AssimpModel* shovelModel;
	AssimpModel* seedBagModel;

	Skybox* skybox;
	Image2d* testUI; //TODO to be removed

	// this is a temp thing until we get animation from server;
	chrono::system_clock::time_point startTime;

	// Particle effect
	ParticleFactory* particleFactory;
	ShaderProgram* particleProgram;

public:

	std::unordered_map<uint, SceneNode*> objectIdMap;
	std::unordered_map<uint, RenderController*> controllers;

	Scene();
	~Scene();

	void setupDirectionalLighting(glm::vec3 eyePosition);
	void setState(GameState * state);
	void update(); // recalcuate the matrices
	void draw(const glm::mat4 &veiwProjMat); // renders everything

	void toggleWater();

	SceneNode* getDrawableSceneNode(uint objectId, Drawable* model);

	// static function for a to create a specfic scene good for grahics testing
	static Scene* scene0();


	// getter & setter
	SceneNode* getGroundNode() { return groundNode;  }
	ParticleFactory* getParticleFactory() { return particleFactory; }
	uint getShaderID(ShaderType type) {
		switch (type) {
			case ShaderType::DEFAULT:			return program->GetProgramID();
			case ShaderType::ASSIMP:			return assimpProgram->GetProgramID();
			case ShaderType::ANIMATED:			return animationProgram->GetProgramID();
			case ShaderType::SKYBOX:			return skyboxProgram->GetProgramID();
			case ShaderType::UI:				return uiProgram->GetProgramID();
			case ShaderType::HEALTH_BAR:		return barProgram->GetProgramID();
		}
	}

	Drawable* getModel(ModelType type) {
		switch (type) {
			case ModelType::CAT:				return playerModel;
			case ModelType::PIG:				return playerModel;
			case ModelType::RABBIT:				return zombieModel;
			case ModelType::CORN:				return cornModel;
			case ModelType::WATER_TAP:			return tapModel;
			case ModelType::WATERING_CAN:		return wateringCanModel;
			case ModelType::SEED_SHACK:			return seedSourceModel;
			case ModelType::SHOVEL:				return shovelModel;
			case ModelType::SEED_BAG:			return seedBagModel;
			case ModelType::SEED:				return seedModel;
			case ModelType::SAPLING:			return saplingModel;
			case ModelType::BABY_CORN:			return babyCornModel;
		}
	}
};

