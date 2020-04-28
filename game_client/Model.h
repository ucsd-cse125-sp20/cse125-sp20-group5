////////////////////////////////////////
// Model.h
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "Drawable.h"
#include "SceneNode.h"

////////////////////////////////////////////////////////////////////////////////

struct ModelVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 Texture;
};

////////////////////////////////////////////////////////////////////////////////

class Model : public Drawable {
public:
	Model(uint shader);
	~Model();

	void draw(SceneNode& node,const glm::mat4 &viewProjMtx) override;
	void update(SceneNode * node) override;
	SceneNode* createSceneNodes(uint objectId) override;

	void makeBox(const glm::vec3 &boxMin,const glm::vec3 &boxMax);
	void makeTile(const glm::vec3& tileMin, const glm::vec3& tileMax, const glm::vec3& color, const char* texturefile);
	void setBuffers(const std::vector<ModelVertex> &vtx,const std::vector<uint> &idx, const char* texturefile);

private:
	uint VertexBuffer;
	uint IndexBuffer;
	uint TextureBuffer;

	int Count;

	uint shader;
};

////////////////////////////////////////////////////////////////////////////////
