#pragma once
//#include "Drawable.h"
#include "core.h"
#include <unordered_map>

class Drawable;

class SceneNode
{
private:

	/*not sure baout this*/
	uint modelId;

	uint objectId;
	Drawable * obj;
	SceneNode* parent;
	std::string name;

	// maybe jsut a vector
	uint childNum;
	uint childCount;

public:

	glm::mat4 transform;
	std::unordered_map<uint, SceneNode*> children;

	SceneNode(Drawable * myO, std::string name, uint objectId);

	// this kills your children to remove yourself if you want your children
	~SceneNode();

	// this take you out of the graph completely set your kids to your parent porbably not used
	void removeSelf();

	// straight forward;
	void setParent(SceneNode * newParent);

	void addChild(SceneNode* newChild);

	std::string getName();

};

