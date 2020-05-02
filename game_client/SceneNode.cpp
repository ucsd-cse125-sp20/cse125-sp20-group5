#include "SceneNode.h"
#include <glm\gtx\euler_angles.hpp>
#include <iostream>

SceneNode::SceneNode(Drawable* myO, std::string name, uint objectId)
{
	obj = myO;
	this->name = name;
	this->objectId = objectId;
	childCount = 0;
	transform = glm::mat4(1.0);
	parent = NULL;
	updated = false; // dont' double update stuff

	animationId = 0;
	animPlayedTime = 0.0;
	numAnimation = 0;

	position = glm::vec3(0.0);
	dir = 0.0;
	scaler = 1.0;
}

SceneNode::~SceneNode()
{
	removeSelf();
	std::unordered_map<uint, SceneNode*>::iterator children;
	for (children = this->children.begin(); children != this->children.end(); children++) {
		delete children->second;
	}
}

void SceneNode::removeSelf()
{
	if (parent != NULL) {
		parent->children.erase(childNum); // erase child from map
		childNum = 0; // reset child num
		parent = NULL; // resert parent
	}
}

void SceneNode::setParent(SceneNode* newParent)
{
	newParent->addChild(this); // call add child but reverse
}

void SceneNode::addChild(SceneNode* newChild)
{
	newChild->removeSelf(); // make sure parent is safely removed 
	newChild->childNum = childCount;  // give th right child id
	childCount++; // increment child id counter

	children[newChild->childNum] = newChild; // set the child
	newChild->parent = this; // set the parent
}

void SceneNode::calcLocalTransform()
{
	transform = glm::mat4(scaler);
	transform[3][3] = 1.0;

	transform = transform * glm::eulerAngleZ(dir);

	transform[3][0] = position[0];
	transform[3][1] = position[1];
	transform[3][2] = position[2];
}

void SceneNode::update(glm::mat4 world)
{
	updateAnimation();// TODO: to be removed if updating animation on the server side

	if (obj != NULL) {
		obj->update(this);
	}

	// if updated externally by another animation function;
	if (!updated) {
		calcLocalTransform();

	}

	transform = world * transform;
	updated = true;

	std::unordered_map<uint, SceneNode*>::iterator it;
	for (it = children.begin(); it != children.end(); it++) {
		it->second->update(transform);
	}
}

void SceneNode::draw(const glm::mat4& veiwProjMat)
{
	updated = false;
	// need to do next load the save bone state in here
	// actually stack the transfroms
	// mark stuff with update 
	if (obj != NULL) {
		obj->draw(*this, veiwProjMat);
	}
	std::unordered_map<uint, SceneNode*>::iterator it;
	for (it = children.begin(); it != children.end(); it++) {
		it->second->draw(veiwProjMat);
	}
}

void SceneNode::loadGameObject(GameObject* gameObj) 
{
	position[0] = gameObj->position->x;
	position[1] = gameObj->position->y;
	position[2] = gameObj->position->z;

	dir = gameObj->direction->angle;

	// TODO if use server to update anim, uncomment the below and delete swtichAnim
	//animationId = gameObj->animation->animationType;
	//animPlayedTime = gameObj->animation->animationFrame;
	switchAnim(gameObj->animation->animationType);
} 

SceneNode* SceneNode::find(std::string name, uint objectId)
{
	if (objectId != this->objectId) {
		return nullptr;
	}
	if (this->name == name) {
		return this;
	}
	std::unordered_map<uint, SceneNode*>::iterator it;
	for (it = children.begin(); it != children.end(); it++) {
		SceneNode* node = it->second->find(name, objectId);
		if (node != nullptr) {
			return node;
		}
	}
	return nullptr;
}

std::string SceneNode::getName() const
{
	return name;
}

/* animation related done on client side */
// TODO: to be removed if updating animation on the server side
void SceneNode::updateAnimation() {
	if (this->numAnimation <= 0) {
		return;
	}
	
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - this->animStartTime;
	this->animPlayedTime = elapsed_seconds.count();
}

void SceneNode::loadAnimData(uint numAnim, uint initialAnimID) {
	this->numAnimation = numAnim;
	this->animationId = initialAnimID;
	this->animStartTime = std::chrono::system_clock::now();
}

void SceneNode::switchAnim(uint newAnimID) {
	if (this->numAnimation <= 0) {
		return; // meaning no animation for this node/model
	}
	if (newAnimID >= this->numAnimation) {
		std::cerr << "Scene node " << this->name 
			<< " does not have animation ID " << newAnimID 
			<< " (has only " << this->numAnimation <<" animations)" << std::endl;
		return;
	}
	if (this->animationId == newAnimID) {
		return; // no need to switch
	}

	this->animationId = newAnimID;
	this->animStartTime = std::chrono::system_clock::now();
}

