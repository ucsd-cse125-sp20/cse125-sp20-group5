#include "Particle.h"
#include <iostream>

Particle::Particle(GLuint shader, glm::mat4 modelMatrix, glm::vec3 color,
	glm::vec3 initialVelocity, glm::vec3 acceleration, int lifeSpan, float maxDistance)
{
    this->lifeLeft = lifeSpan;
    this->velocity = initialVelocity;
    this->acceleration = acceleration;
    
    this->shader = shader;
    
	this->individualModelMatrix = modelMatrix;
    
	// The color of the particle
	this->color = color;

    this->distanceLeft = maxDistance;
}

Particle::~Particle()
{
    // Clean up
}

void Particle::draw(glm::mat4 model, const glm::mat4& viewProjMat, GLuint parentVAO)
{
    glm::mat4 modelMtx = individualModelMatrix * model;
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProjMat"), 1, GL_FALSE, glm::value_ptr(viewProjMat));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(modelMtx));
    glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));
    
	// Bind to the VAO of the parrent.
	glBindVertexArray(parentVAO);
	// Draw triangles using the indices in the second VBO, which is an 
	// elemnt array buffer.
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // TODO: 36?
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void Particle::update(float timeDifference)
{
    // Displacement
    glm::vec3 d = velocity * timeDifference;
    // Add distance to traveled distance
    distanceLeft -= glm::length(d);
    // Translate the particle
    individualModelMatrix = glm::translate(individualModelMatrix, d);

    // update life and V
    lifeLeft -= timeDifference;
    velocity += timeDifference * acceleration;
}

bool Particle::isAlive()
{
    if (lifeLeft > 0.0f && distanceLeft > 0.0f ) return true;
    return false;
}

