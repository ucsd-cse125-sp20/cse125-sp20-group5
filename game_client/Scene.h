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
#include "HealthBar.h"
#include "Image2d.h" //TODO to be removed
#include "TextUI.h" //TODO to be removed
#include "Constants.h"
#include "Shader.h"
#include "ParticleFactory.hpp"
#include "ClientParams.h"
#include "AudioEngine.h"

class RenderController; //  put declaration here to sidestep header issues

enum class ShaderType {
	DEFAULT, ASSIMP, ANIMATED, SKYBOX, TEXT, UI, HEALTH_BAR
};

enum class ModelType {
	CAT, BLACKPIG, TIGER, CHICKEN, RABBIT, PIG,
	WATER_TAP, WATERING_CAN, SEED_SHACK_CORN, SEED_SHACK_CACTUS, SHOVEL, SEED_BAG, SPRAY, FERTILIZER,
	CORN, SEED, SAPLING, BABY_CORN, BABY_CACTUS, CACTUS, CACTUS_BULLET,
	HOME_BASE, BABY_PLAYER_PLANT
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
	ShaderProgram* textProgram;
	ShaderProgram* barProgram;

	// models
	AnimatedAssimpModel* zombieModel;
	AnimatedAssimpModel* zombiePigModel;
	AnimatedAssimpModel* playerTigerModel;
	AnimatedAssimpModel* playerCatModel;
	AnimatedAssimpModel* playerPigModel;
	AnimatedAssimpModel* playerChickenModel;

	AssimpModel* seedModel;
	AssimpModel* saplingModel;
	AssimpModel* babyplayerplant;
	AssimpModel* babyCornModel;
	AssimpModel* cornModel;
	AssimpModel* babyCactusModel;
	AssimpModel* cactusModel;
	AssimpModel* cactusBulletModel;
	AssimpModel* tapModel;
	AssimpModel* wateringCanModel;
	AssimpModel* seedSourceModel_corn;
	AssimpModel* seedSourceModel_cactus;
	AssimpModel* shovelModel;
	AssimpModel* seedBagModel;
	AssimpModel* baseModel;
	AssimpModel* sprayModel;
	AssimpModel* fertilizerModel;
	AssimpModel* treeModel;

	std::vector<AssimpModel*> rockModels;

	Skybox* skybox;
	Image2d* testUI; //TODO to be removed
	TextUI* textUI; //TODO to be removed
	TextUI* textInWorld; //TODO to be removed

	// this is a temp thing until we get animation from server;
	chrono::system_clock::time_point startTime;

	// Particle effect
	ParticleFactory* particleFactory;
	ShaderProgram* particleProgram;

	int zombieWaveNum = 0;
	int totalWaveNum = 0;

public:
	ClientParams& config;

	std::unordered_map<uint, SceneNode*> objectIdMap;
	std::unordered_map<uint, RenderController*> controllers;

	Scene(ClientParams& config);
	~Scene();

	void setupDirectionalLighting(glm::vec3 eyePosition);
	void setState(GameState * state);
	void update(); // recalcuate the matrices
	void draw(const glm::mat4 &veiwProjMat); // renders everything

	SceneNode* getDrawableSceneNode(uint objectId, Drawable* model);

	// static function for a to create a specfic scene good for grahics testing
	static Scene* scene0(ClientParams& config);
	
	// passed in by Client
	CAudioEngine* aEngine = nullptr;
	float volumeAdjust;

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
			case ShaderType::TEXT:				return textProgram->GetProgramID();
			case ShaderType::HEALTH_BAR:		return barProgram->GetProgramID();
		}
	}

	Drawable* getModel(ModelType type) {
		switch (type) {
			case ModelType::CAT:				return playerCatModel;
			case ModelType::TIGER:				return playerTigerModel;
			case ModelType::CHICKEN:			return playerChickenModel;
			case ModelType::BLACKPIG:			return playerPigModel;
			case ModelType::PIG:				return zombiePigModel;
			case ModelType::RABBIT:				return zombieModel;
			case ModelType::CORN:				return cornModel;
			case ModelType::WATER_TAP:			return tapModel;
			case ModelType::WATERING_CAN:		return wateringCanModel;
			case ModelType::SEED_SHACK_CORN:			return seedSourceModel_corn;
			case ModelType::SEED_SHACK_CACTUS:			return seedSourceModel_cactus;
			case ModelType::SHOVEL:				return shovelModel;
			case ModelType::SEED_BAG:			return seedBagModel;
			case ModelType::SEED:				return seedModel;
			case ModelType::SAPLING:			return saplingModel;
			case ModelType::BABY_CORN:			return babyCornModel;
			case ModelType::BABY_CACTUS:		return babyCactusModel;
			case ModelType::CACTUS:				return cactusModel;
			case ModelType::CACTUS_BULLET:		return cactusBulletModel;
			case ModelType::HOME_BASE:			return baseModel;
			case ModelType::SPRAY:				return sprayModel;
			case ModelType::FERTILIZER:			return fertilizerModel;
			case ModelType::BABY_PLAYER_PLANT:	return babyplayerplant;
		}
	}
};

