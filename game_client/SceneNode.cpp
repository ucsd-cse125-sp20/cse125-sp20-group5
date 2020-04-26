#include "SceneNode.h"

SceneNode::SceneNode(Drawable* myO, std::string name, uint objectId)
{
	obj = myO;
	this->name = name;
	this->objectId = objectId;
	childCount = 0;
	parent = NULL;
}

SceneNode::~SceneNode()
{
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

std::string SceneNode::getName() 
{
	return name;
}
