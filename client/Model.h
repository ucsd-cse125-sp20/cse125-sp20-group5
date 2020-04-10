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
	glm::vec2 Texture;
};

////////////////////////////////////////////////////////////////////////////////

class Model {
public:
	Model();
	~Model();

	void draw(const glm::mat4 &modelMtx,const glm::mat4 &viewProjMtx,uint shader);
	void update();

	void makeBox(const glm::vec3 &boxMin,const glm::vec3 &boxMax);
	void makeTile(const glm::vec3& tileMin, const glm::vec3& tileMax, const glm::vec3& color, char* texturefile);
	void setBuffers(const std::vector<ModelVertex> &vtx,const std::vector<uint> &idx, char* texturefile);

	glm::mat4 getLocalMat();
	void setPose(glm::vec3 pose);
	void setPosition(glm::vec3 position);
	glm::vec3 getPosition();

private:
	uint VertexBuffer;
	uint IndexBuffer;
	uint TextureBuffer;

	glm::vec3 pose;
	glm::vec3 position;
	glm::mat4 localMat;

	int Count;
};

////////////////////////////////////////////////////////////////////////////////
