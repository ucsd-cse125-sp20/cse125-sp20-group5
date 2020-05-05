#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "Core.h"
#include "Cube.h"
#include "stb_image.h"

class Skybox
{
public:
	Skybox(uint shader, glm::mat4 modelMtx);
	~Skybox();

	void draw(const glm::mat4& viewProjMtx);
	void loadTexture();

private:
	glm::mat4 toWorld;
	std::vector<std::string> filePaths;
	unsigned int textureID;
	uint shader;
	glm::mat4 modelMtx;

	GLuint VBO, VAO, EBO;
};

