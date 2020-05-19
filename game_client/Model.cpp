////////////////////////////////////////
// Model.cpp
////////////////////////////////////////

#include "Model.h"
#include "glm/gtx/euler_angles.hpp"
#include <iostream>

// get for image import
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

////////////////////////////////////////////////////////////////////////////////

Model::Model(uint shader) {
	this->shader = shader;

	glGenBuffers(1, &VertexBuffer);
	glGenBuffers(1, &IndexBuffer);
	glGenTextures(1, &TextureBuffer);

	Count = 0;
}

////////////////////////////////////////////////////////////////////////////////

Model::~Model() {
	glDeleteBuffers(1, &IndexBuffer);
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteTextures(1, &TextureBuffer);
}

////////////////////////////////////////////////////////////////////////////////

void Model::draw(SceneNode& node, const glm::mat4& viewProjMtx) {
	// Set up shader
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMtx"), 1, false, (float*)&(node.transform));

	glm::mat4 mvpMtx = viewProjMtx * node.transform;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjMtx"), 1, false, (float*)&mvpMtx);

	// Set up state
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

	uint posLoc = 0;
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), 0);

	uint normLoc = 1;
	glEnableVertexAttribArray(normLoc);
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)12);

	uint colorLoc = 2;
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)24);

	uint texLoc = 3;
	glEnableVertexAttribArray(texLoc);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)36);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, TextureBuffer);

	// Draw geometry
	glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, 0);

	// Clean up state
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(colorLoc);
	glDisableVertexAttribArray(texLoc);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

void Model::update(SceneNode * node)
{

}

SceneNode* Model::createSceneNodes(uint objectId)
{
	return new SceneNode(this, std::string("model"), objectId);
}

////////////////////////////////////////////////////////////////////////////////

void Model::makeBox(const glm::vec3& boxMin, const glm::vec3& boxMax) {
	// Specify vertices
	std::vector<ModelVertex> vtx = {
		// Front
		{{boxMin.x,boxMin.y,boxMax.z},	{0,0,1}, {0,1.0,0}, {0,0}},
		{{boxMax.x,boxMin.y,boxMax.z},	{0,0,1}, {0,1.0,0}, {1.0,0}},
		{{boxMax.x,boxMax.y,boxMax.z},	{0,0,1}, {0,1.0,0}, {1.0,1.0}},
		{{boxMin.x,boxMax.y,boxMax.z},	{0,0,1}, {0,1.0,0}, {0,1.0}},

		// Back
		{{boxMax.x,boxMin.y,boxMin.z},	{0,0,-1}, {1.0,0,0}, {1.0,0}},
		{{boxMin.x,boxMin.y,boxMin.z},	{0,0,-1}, {1.0,0,0}, {0,0}},
		{{boxMin.x,boxMax.y,boxMin.z},	{0,0,-1}, {1.0,0,0}, {0,1.0}},
		{{boxMax.x,boxMax.y,boxMin.z},	{0,0,-1}, {1.0,0,0}, {1.0,1.0}},

		// Top
		{{boxMin.x,boxMax.y,boxMax.z},	{0,1,0}, {0,0,1.0}, {0,1.0}},
		{{boxMax.x,boxMax.y,boxMax.z},	{0,1,0}, {0,0,1.0}, {1.0,1.0}},
		{{boxMax.x,boxMax.y,boxMin.z},	{0,1,0}, {0,0,1.0}, {1.0,0}},
		{{boxMin.x,boxMax.y,boxMin.z},	{0,1,0}, {0,0,1.0}, {0,0}},

		// Bottom
		{{boxMin.x,boxMin.y,boxMin.z},	{0,-1,0}, {0,0,0}, {0,0}},
		{{boxMax.x,boxMin.y,boxMin.z},	{0,-1,0}, {0,0,0}, {0,0}},
		{{boxMax.x,boxMin.y,boxMax.z},	{0,-1,0}, {0,0,0}, {0,0}},
		{{boxMin.x,boxMin.y,boxMax.z},	{0,-1,0}, {0,0,0}, {0,0}},

		// Left
		{{boxMin.x,boxMin.y,boxMin.z},	{-1,0,0}, {0,0,1.0}, {0,0}},
		{{boxMin.x,boxMin.y,boxMax.z},	{-1,0,0}, {0,0,1.0}, {0,0}},
		{{boxMin.x,boxMax.y,boxMax.z},	{-1,0,0}, {0,0,1.0}, {0,0}},
		{{boxMin.x,boxMax.y,boxMin.z},	{-1,0,0}, {0,0,1.0}, {0,0}},

		// Right
		{{boxMax.x,boxMin.y,boxMax.z},	{1,0,0}, {0,1.0,0}, {0,0}},
		{{boxMax.x,boxMin.y,boxMin.z},	{1,0,0}, {0,1.0,0}, {0,0}},
		{{boxMax.x,boxMax.y,boxMin.z},	{1,0,0}, {0,1.0,0}, {0,0}},
		{{boxMax.x,boxMax.y,boxMax.z},	{1,0,0}, {0,1.0,0}, {0,0}},
	};

	// Specify indices
	std::vector<uint> idx = {
		0,1,2,		0,2,3,			// Front
		4,5,6,		4,6,7,			// Back
		8,9,10,		8,10,11,		// Top
		12,13,14,	12,14,15,		// Bottom
		16,17,18,	16,18,19,		// Left
		20,21,22,	20,22,23,		// Right
	};

	// Create vertex & index buffers
	setBuffers(vtx, idx, NO_TEXTURE);
}

void Model::makeTile(const glm::vec3& tileMin, const glm::vec3& tileMax, const glm::vec3& color, const char * texturefile) {
	// Specify vertices
	std::vector<ModelVertex> vtx = {
		// Front
		{{tileMin.x,tileMax.y,tileMax.z},	{0,1,0}, color, {0,1.0}},
		{{tileMax.x,tileMax.y,tileMax.z},	{0,1,0}, color, {1.0,1.0}},
		{{tileMax.x,tileMax.y,tileMin.z},	{0,1,0}, color, {1.0,0}},
		{{tileMin.x,tileMax.y,tileMin.z},	{0,1,0}, color, {0,0}},
	};

	// Specify indices
	std::vector<uint> idx = {
		0,1,2,		0,2,3,			// Front
	};

	// Create vertex & index buffers
	setBuffers(vtx, idx, texturefile);
}

void Model::makeOutline(float xSize, float ySize, float lineWidth, const glm::vec3& color) {
	std::vector<ModelVertex> vtx = {
		{{0.0,0.0,0.0},	{0,1,0}, color, {0,1.0}},
		{{0.0,0.0,ySize},	{0,1,0}, color, {1.0,1.0}},
		{{xSize,0.0,0.0},	{0,1,0}, color, {1.0,0}},
		{{xSize,0.0,ySize},	{0,1,0}, color, {0,0}},

		{{lineWidth,0.0,lineWidth},	{0,1,0}, color, {0,1.0}},
		{{lineWidth,0.0,ySize-lineWidth},	{0,1,0}, color, {1.0,1.0}},
		{{xSize-lineWidth,0.0,lineWidth},	{0,1,0}, color, {1.0,0}},
		{{xSize-lineWidth,0.0,ySize-lineWidth},	{0,1,0}, color, {0,0}},

		{{0.0,0.0,lineWidth},	{0,1,0}, color, {0,1.0}},
		{{xSize,0.0,lineWidth},	{0,1,0}, color, {1.0,1.0}},
		{{0.0,0.0,ySize-lineWidth},	{0,1,0}, color, {1.0,0}},
		{{xSize,0.0,ySize - lineWidth},	{0,1,0}, color, {0,0}},
	};

	// Specify indices
	std::vector<uint> idx = {
		0,8,2,     8,9,2,    // 0 2 8 9
		10,1,3,    10,3,11,   // 10 1 3 11
		4,8,5,     5,8,10,   // 8 4 5 10
		6,7,9,     9,7,11    // 6 7 9 11
	};

	setBuffers(vtx, idx, NO_TEXTURE);
}

////////////////////////////////////////////////////////////////////////////////

void Model::setBuffers(const std::vector<ModelVertex>& vtx, const std::vector<uint>& idx, const char * texturefile) {
	Count = (int)idx.size();

	// Store vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(ModelVertex), &vtx[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Store index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(uint), &idx[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//store texture buffer
	glBindTexture(GL_TEXTURE_2D, TextureBuffer);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturefile, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << texturefile << std::endl;
	}
	stbi_image_free(data);
}