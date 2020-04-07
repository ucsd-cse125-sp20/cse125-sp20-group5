////////////////////////////////////////
// Model.h
////////////////////////////////////////

#pragma once

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////

struct ModelVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
};

////////////////////////////////////////////////////////////////////////////////

class Model {
public:
	Model();
	~Model();

	void draw(const glm::mat4 &modelMtx,const glm::mat4 &viewProjMtx,uint shader);
	void update();

	void makeBox(const glm::vec3 &boxMin,const glm::vec3 &boxMax);
	void setBuffers(const std::vector<ModelVertex> &vtx,const std::vector<uint> &idx);

	glm::mat4 getLocalMat();
	void setPose(glm::vec3 pose);
	void setPosition(glm::vec3 position);
	glm::vec3 getPosition();

private:
	uint VertexBuffer;
	uint IndexBuffer;

	glm::vec3 pose;
	glm::vec3 position;
	glm::mat4 localMat;

	int Count;
};

////////////////////////////////////////////////////////////////////////////////
