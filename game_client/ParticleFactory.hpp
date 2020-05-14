#pragma once

#include "ParticleGroup.h"

class ParticleFactory {
private:
	GLuint shaderID;
public:
	ParticleFactory(GLuint shaderID) {
		this->shaderID = shaderID;
	}

	~ParticleFactory() {
		// Delete
	}

	ParticleGroup * getWaterTapParticleGroup(glm::vec3 particlePosition) {
		// The color of the particles
		glm::vec3 particleColor = glm::vec3(0.2f, 0.95f, 1.0f);
		// The initial velocity of the newly spawned particles
		glm::vec3 particleInitVelocity(0.0f, -.5f, 0.0f);
		// The acceleration of the particles, gravity etc
		glm::vec3 particleAcceleration(0.0f, -0.3f, 0.0f);
		// How many particles are initialized with the particle group at the
		// very beginning
		int particleInitNum = 1;
		// How many particles are rendered on the screen at the same time
		int particleMaxNum = 50;
		// How long each individual particle could live
		float particleLifeSpan = 2.0f;
		// How much could each individual's color differ from each other randomly
		glm::vec3 colorVariance(0.1f, 0.1f, 0.1f);
		// How much could each individual's initial velocity differ from each other randomly
		glm::vec3 velocityVariance(0.1f, 0.1f, 0.1f);
		// The size of the particles
		float particleSize = 0.2f;
		// How fast to spawn a new particle
		float spawnTime = .1f;
		int spawnNum = 1;
		float maxDistance = 1000;
		return new ParticleGroup(shaderID,
			particleSize, particlePosition, particleColor, particleInitVelocity,
			particleAcceleration, particleInitNum, particleMaxNum, particleLifeSpan,
			colorVariance, velocityVariance, spawnTime, spawnNum, maxDistance);
	}

	ParticleGroup* getCornAttackParticleGroup(glm::vec3 particlePosition, float maxDistance) {
		// The color of the particles
		glm::vec3 particleColor = glm::vec3(1.0f, 0.95f, 0.2f);
		// The initial velocity of the newly spawned particles
		glm::vec3 particleInitVelocity(0.0f, 0.0f, 0.0f);
		// The acceleration of the particles, gravity etc
		glm::vec3 particleAcceleration(0.0f, -0.3f, 0.0f);
		// How many particles are initialized with the particle group at the
		// very beginning
		int particleInitNum = 1;
		// How many particles are rendered on the screen at the same time
		int particleMaxNum = 100;
		// How long each individual particle could live
		float particleLifeSpan = 1000.0f;
		// How much could each individual's color differ from each other randomly
		glm::vec3 colorVariance(0.1f, 0.1f, 0.1f);
		// How much could each individual's initial velocity differ from each other randomly
		glm::vec3 velocityVariance(1.0f, 0.0f, 1.0f);
		// The size of the particles
		float particleSize = 0.2f;
		// How fast to spawn a new particle
		float spawnTime = 0.01f;
		int spawnNum = 1;
		return new ParticleGroup(shaderID,
			particleSize, particlePosition, particleColor, particleInitVelocity,
			particleAcceleration, particleInitNum, particleMaxNum, particleLifeSpan,
			colorVariance, velocityVariance, spawnTime, spawnNum, maxDistance);
	}
};