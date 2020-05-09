#ifndef _Particle_Group_H_
#define _Particle_Group_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Particle.h"
#include "Drawable.h"
#include "SceneNode.h"

/**
    This class would be responsable for creating, managing, rendering a group of particles.
*/
class ParticleGroup : public Drawable
{
private:
    std::vector<Particle*> children;

	GLuint vao; // Vao for paritcles
	GLuint vbos[2]; // Vbo for paricles
    GLuint shader; // The shader that renders the particles

    glm::mat4 groupModelMatrix; // Model matrix of the whole particle group

    glm::vec3 baseColor; // Color of the children particles
    glm::vec3 initialVelocity; // The current velocity of each particle

    glm::vec3 acceleration; // How the velocity of particles changes every update

    int maxParticleNum; // Maximum number of particles rendered
    float particleLifeSpan; // The lifespan of each particle

    // Those variables are used to randomize each particle
    glm::vec3 colorVariance;
    glm::vec3 initialVelocityVariance;

    // Helper functions
    glm::vec3 randomizeVec3(glm::vec3 base, glm::vec3 variance);
    void addChildParticle();

    float spawnTime;
    float timePastSinceLastSpawn;
    int spawnNum;

    bool spawning;

    float particleSize;

public:
	ParticleGroup(GLuint shader, float particleSize, glm::vec3 particlePosition,
        glm::vec3 color, glm::vec3 initialVelocity,
        glm::vec3 acceleration,  int initParicleNum, int maxParticleNum, float lifeSpan,
        glm::vec3 colorVariance, glm::vec3 initialVelocityVariance,
        float spawnTime, int spawnNum);
    ~ParticleGroup();
    
    // Draw the whole particle group
    void draw(SceneNode& node, const glm::mat4& viewProjMatrix) override;
	void update(SceneNode * node) override;
    SceneNode * createSceneNodes(uint objectId) override;
    void toggleSpawning();
    bool isSpawning();
};

#endif

