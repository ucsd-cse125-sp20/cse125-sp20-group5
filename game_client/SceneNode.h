#pragma once

#include "Drawable.h"
#include "Core.h"
#include <unordered_map>
#include <GameObject.hpp>

class SceneNode
{
private:

	/*not sure baout this*/
	uint modelId;
	uint childNum;
	uint childCount;
	bool hasParticleBool;

public:

	std::string name;
	uint objectId;
	glm::mat4 transform;
	SceneNode* parent;
	std::unordered_map<uint, SceneNode*> children;
	Drawable* obj;

	bool updated;
	glm::vec3 position;
	glm::vec3 pose;
	float scaler;
	bool highlighted;

	SceneNode(Drawable * myO, std::string name, uint objectId);

	// this kills your children to remove yourself if you want your children
	~SceneNode();

	// this only kill nodes with your oj=bjectid;
	void deleteSelf();

	// this take you out of the graph completely set your kids to your parent porbably not used
	void removeSelf();

	// straight forward;
	void setParent(SceneNode * newParent);
	void addChild(SceneNode* newChild);
	void calcLocalTransform();

	// update the matrices recursive
	void update(glm::mat4 world);

	// draw the matrices recursive
	void draw(const glm::mat4& veiwProjMat);

	void loadGameObject(GameObject * gameObj);
	
	int countChildern();
	SceneNode* findHand(uint objectId);
	// finds a sub noe with the given name and object id
	// used for finding hands and such 
	SceneNode* find(std::string name, uint objectId);


	/* animation related */
	uint numAnimation;  // TODO: to be removed if updating animation on the server side
	uint animationId;
	float animPlayedTime; // how many time has passed since animStartTime
	std::chrono::system_clock::time_point animStartTime;  // TODO: to be removed if updating animation on the server side
	bool loopAnimation = true;
	bool playedOneAnimCycle = false;

	// TODO: to be removed if updating animation on the server side
	void updateAnimation();
	void loadAnimData(uint numAnim, uint initialAnimID, bool alwaysLoop = true);
	void switchAnim(uint newAnimID, bool alwaysLoop = true);

	std::string getName() const;

};

