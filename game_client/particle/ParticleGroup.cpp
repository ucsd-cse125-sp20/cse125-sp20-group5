#include "ParticleGroup.h"

ParticleGroup::ParticleGroup(GLuint shader, glm::mat4 modelMatrix, glm::vec3 color, glm::vec3 initialVelocity,
	glm::vec3 acceleration, int initParicleNum, int maxParticleNum, float lifeSpan,
	glm::vec3 colorVariance, glm::vec3 initialVelocityVariance)
{
	// Setup variables

	this->shader = shader;
	this->groupModelMatrix = modelMatrix;
	this->baseColor = color;
	this->initialVelocity = initialVelocity;
	this->acceleration = acceleration;
	this->maxParticleNum = maxParticleNum;
	this->particleLifeSpan = lifeSpan;

	this->colorVariance = colorVariance;
	this->initialVelocityVariance = initialVelocityVariance;

	// Initialize children
	for (int i = 0; i < initParicleNum; i++) {
		glm::vec3 randColor = randomizeVec3(baseColor, colorVariance);
		glm::vec3 randInitialV = randomizeVec3(initialVelocity, initialVelocityVariance);
		
		children.push_back(
			// TODO: varify identity matrix
			Particle(this->shader, glm::mat4(1.0f), randColor,
			randInitialV, this->acceleration, this->particleLifeSpan)
		);
	}

	// Setup the buffers for rendering

	/*
	 * Cube indices used below.
	 *    4----7
	 *   /|   /|
	 *  0-+--3 |
	 *  | 5--+-6
	 *  |/   |/
	 *  1----2
	 *
	 */

	 // The 8 vertices of a cube.
	std::vector<glm::vec3> vertices
	{
		glm::vec3(-1, 1, 1),
		glm::vec3(-1, -1, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(-1, 1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, 1, -1)
	};

	// Each ivec3(v1, v2, v3) define a triangle consists of vertices v1, v2 
	// and v3 in counter-clockwise order.
	std::vector<glm::ivec3> indices
	{
		// Front face.
		glm::ivec3(0, 1, 2),
		glm::ivec3(2, 3, 0),
		// Back face.
		glm::ivec3(7, 6, 5),
		glm::ivec3(5, 4, 7),
		// Right face.
		glm::ivec3(3, 2, 6),
		glm::ivec3(6, 7, 3),
		// Left face.
		glm::ivec3(4, 5, 1),
		glm::ivec3(1, 0, 4),
		// Top face.
		glm::ivec3(4, 0, 3),
		glm::ivec3(3, 7, 4),
		// Bottom face.
		glm::ivec3(1, 5, 6),
		glm::ivec3(6, 2, 1),
	};

	// Initialize the vao and vbos

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbos);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO. We will use it to store the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	// Pass in the data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

ParticleGroup::~ParticleGroup()
{
	// Delete the VBOs and the VAO.
	glDeleteBuffers(2, vbos);
	glDeleteVertexArrays(1, &vao);
}

glm::vec3 ParticleGroup::randomizeVec3(glm::vec3 base, glm::vec3 variance)
{
	// Example: v1 = rand() % 100; // v1 in the range 0 to 99
	// Randomize floats between -1 and 1
	// [0, 200] ---(-100)---> [-100, 100] ----(/100)----> [-1.0, 1.0]
	// TODO: verify math
	float xrand = (float)(rand() % 201 - 100) / 100.0f;
	float yrand = (float)(rand() % 201 - 100) / 100.0f;
	float zrand = (float)(rand() % 201 - 100) / 100.0f;

	// TODO: delete
	std::cout << "randomized particle: " << xrand << " " << yrand << " " << zrand << "\n";

	glm::vec3 diff = glm::vec3(xrand * variance.x, yrand * variance.y, zrand * variance.z);

	return base + diff;
}

// Draw all the children
void ParticleGroup::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{

}

void ParticleGroup::update(float timeDifference)
{

}