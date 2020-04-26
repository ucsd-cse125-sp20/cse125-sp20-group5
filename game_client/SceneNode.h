#pragma once
#include "Drawable.h"
#include "core.h"
#include <unordered_map>

class SceneNode
{
private:

	/*not sure baout this*/
	uint modelId;

	uint objectId;
	SceneNode* parent;
	std::string name;

	// maybe jsut a vector
	uint childNum;
	uint childCount;

	// animation info;

public:

	glm::mat4 transform;
	std::unordered_map<uint, SceneNode*> children;
	Drawable* obj;
	uint animationId;
	float animationTime;
	bool updated;
	glm::vec3 position;
	float dir;
	float scaler;

	SceneNode(Drawable * myO, std::string name, uint objectId);

	// this kills your children to remove yourself if you want your children
	~SceneNode();

	// this take you out of the graph completely set your kids to your parent porbably not used
	void removeSelf();

	// straight forward;
	void setParent(SceneNode * newParent);

	void addChild(SceneNode* newChild);

	void calcLocalTransform();

	// update the matrices recursive
	void update(glm::mat4 world);

	// draw teh matrices recursive
	void draw(const glm::mat4& veiwProjMat);

	std::string getName();

};

