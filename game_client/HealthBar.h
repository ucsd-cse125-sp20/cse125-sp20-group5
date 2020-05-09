////////////////////////////////////////
// AnimatedAssimpModel.h
// 
// Class that inherits AssimpModel and adds animation
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "stb_image.h"
#include "Constants.h"


struct BarComponent {
	uint id;
	glm::mat4 localMtx;
	glm::vec3 color;

	BarComponent() {}

	BarComponent(uint id, glm::mat4 localMtx, glm::vec3 color) {
		this->id = id;
		this->localMtx = localMtx;
		this->color = color;
	}
};

enum BarComponentID { ICON, BAR_BOX, BAR };

class HealthBar 
{
public:

    HealthBar(uint shader, const char* texture_file, float initFilledFraction = 1.0f, glm::vec3 barColor = glm::vec3(1.0f));
    ~HealthBar();

	void draw(const glm::mat4& viewProjMtx);
	void updateBar(float filledFraction);

	// These variables are needed for the shader program
	GLuint VBO, VBO2, VAO, EBO;
	GLuint textureIDs[3];
	uint shader;

	glm::mat4 modelMtx;
	//glm::mat4 baseBarTransform;
	glm::mat4 fillingTransform; // scale transform of the bar's filling portion 
	float filledFraction = 0.0; // how much the bar is filled, between 0.0 & 1.0
	float currFilledFraction;

private:

	std::vector<BarComponent> barComponents;
	void loadTexture(const char*, uint id);

	const float fillingStep = 0.01f;

	// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
// This just looks nicer since it's easy to tell what coordinates/indices belong where.
	GLfloat vertices[4][3] = {
		{ -0.5, -0.5,  0.0 },{ -0.5, 0.5,  0.0 },{ 0.5,  0.5, 0.0 },{ 0.5,  -0.5,  0.0 }
	};

	const GLfloat textures[4][2] = {
		{ 0.0, 1.0 },{ 0.0, 0.0 },{ 1.0,  0.0 },{ 1.0,  1.0 }
	};

	// Note that GL_QUADS is deprecated in modern OpenGL (and removed from OSX systems).
	// This is why we need to draw each face as 2 triangles instead of 1 quadrilateral
	const GLuint indices[1][6] = {
		{ 0, 2, 1, 0, 3, 2 }
	};
 
};
