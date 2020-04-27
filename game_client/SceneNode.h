#pragma once
#include "Drawable.h"
#include "core.h"
#include <unordered_map>
#include <GameObject.hpp>

class SceneNode
{
private:

	/*not sure baout this*/
	uint modelId;

	SceneNode* parent;
	std::string name;
	uint childNum;
	uint childCount;

	// animation info;

public:

	uint objectId;
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

	void loadGameObject(GameObject * gameObj);

	// finds a sub noe with the given name and object id
	// used for finding hands and such 
	SceneNode* find(std::string name, uint objectId);

	std::string getName() const;

};

