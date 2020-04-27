#pragma once
#include "core.h"
//#include "SceneNode.h"

class SceneNode;

class Drawable
{
private:
public:
	virtual void draw(SceneNode& node, const glm::mat4& viewProjMtx) = 0;
	virtual void update(SceneNode* node) = 0;
	virtual SceneNode * createSceneNodes(uint objectId) = 0;
};

