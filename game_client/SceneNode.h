#pragma once
#include "Drawable.h"
#include "core.h"
class SceneNode
{
private:
	Drawable* obj;
	SceneNode* parent;
	std::vector<SceneNode*> childern;

public:
	SceneNode(Drawable * myO);

	// this kills your children to remove yourself if you want your children
	~SceneNode();

	// this take you out of the graph completely set your kids to your parent porbably not used
	void removeSelf();

	// this makes more sense just becuase adding a child effects nothing else adding a parent is harder
	void setParent(SceneNode * newParent);
};

