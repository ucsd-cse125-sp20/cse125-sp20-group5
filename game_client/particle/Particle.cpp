#include "Particle.h"

Particle::Particle(GLuint shader, glm::mat4 modelMatrix, glm::vec3 color,
	glm::vec3 initialVelocity, glm::vec3 acceleration, int lifeSpan)
{
    this->lifeLeft = lifeSpan;
    this->velocity = velocity;
    this->acceleration = acceleration;
    
    this->shader = shader;
    
	this->individualModelMatrix = modelMatrix;
    
	// The color of the particle
	this->color = color;
}

Particle::~Particle()
{
    // Clean up
}

void Particle::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, 
    glm::mat4 parentModelMatrix, GLuint parentVAO)
{
    glm::mat4 combinedModelMatrix = parentModelMatrix * individualModelMatrix; // TODO: validate correctness

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(combinedModelMatrix));
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
    lifeLeft -= timeDifference;
    velocity += acceleration;
    individualModelMatrix = glm::translate(individualModelMatrix, velocity);
}

