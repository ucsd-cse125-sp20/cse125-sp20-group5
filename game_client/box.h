#pragma once
#include <vector>
#include "core.h"

class box
{
private:
	glm::mat4 trans;
public:
	box();
	void update(glm::mat4 world);
	void draw(glm::mat4 world);
	void adjustPose(float index, float dir);
	glm::mat4 getTransform();
	glm::vec3 offset;
	glm::vec3 pose;
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 rotmin;
	glm::vec3 rotmax;
	char name[128];
	std::vector<box*> childern;
};

