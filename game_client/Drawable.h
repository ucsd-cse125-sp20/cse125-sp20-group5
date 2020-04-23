#pragma once
#include "core.h"

class Drawable
{
private:
public:
	virtual void update() = 0;
	virtual void draw(const glm::mat4& viewProjMtx) = 0;
};

