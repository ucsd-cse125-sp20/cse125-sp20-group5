#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "Core.h"
#include "stb_image.h"

class Image2d
{
public:
	Image2d(uint shader, const char* texture_file, float factor, glm::vec2 offset, int minType, float thres, bool fillx = false);
	~Image2d();

	glm::mat4 toWorld;

	void draw();
	void loadTexture(const char*);
	void updateScale(float, float, glm::vec2 = glm::vec2(0));

	// These variables are needed for the shader program
	GLuint VBO, VBO2, VAO, EBO;
	GLuint uProjection, uModelview, uObjectType;
	GLuint texture[1];
	uint shader;

	glm::vec2 curOffset;
	bool fillx;
	glm::vec2 prevWindow;
	int minifyType;
	float threshold;

private:
	// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
	// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
	// This just looks nicer since it's easy to tell what coordinates/indices belong where.
	GLfloat vertices[4][3] = {
		{ -1.0, -1.0,  0.0 },{ -1.0, 1.0,  0.0 },{ 1.0,  1.0, 0.0 },{ 1.0,  -1.0,  0.0 }
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

