#pragma once
//#include "Drawable.h"
#include "core.h"
#include <unordered_map>

class Drawable;

class SceneNode
{
private:

	/*not sure baout this*/
	uint objectId;
	uint modelId;

	Drawable * obj;
	SceneNode* parent;
	std::string name;

	glm::mat4 transform;

	// maybe jsut a vector
	uint childNum;
	uint childCount;
	std::unordered_map<uint, SceneNode*> childern;

public:
	SceneNode(Drawable * myO, std::string name, uint objectId);

	// this kills your children to remove yourself if you want your children
	~SceneNode();

	// this take you out of the graph completely set your kids to your parent porbably not used
	void removeSelf();

	// straight forward;
	void setParent(SceneNode * newParent);


	void addChild(SceneNode* newChild);
};

