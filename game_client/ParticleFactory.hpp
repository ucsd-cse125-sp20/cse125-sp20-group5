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

	ParticleGroup * getWaterTapParticleGroup() {
		glm::vec3 particleColor = glm::vec3(0.2f, 0.95f, 1.0f);
		glm::vec3 particleInitVelocity(0.0f, 0.0f, 0.0f);
		glm::vec3 particleAcceleration(0.0f, -0.3f, 0.0f);
		int particleInitNum = 1;
		int particleMaxNum = 20;
		float particleLifeSpan = 20.0f;
		glm::vec3 colorVariance(0.1f, 0.1f, 0.1f);
		glm::vec3 velocityVariance(0.1f, 0.1f, 0.1f);
		float particleSize = 0.6f;
		glm::vec3 particlePosition = glm::vec3(0.0f, 0.0f, 0.0f);
		float spawnTime = 1.0f;

		return new ParticleGroup(shaderID,
			particleSize, particlePosition, particleColor, particleInitVelocity,
			particleAcceleration, particleInitNum, particleMaxNum, particleLifeSpan,
			colorVariance, velocityVariance, spawnTime);
	}
};