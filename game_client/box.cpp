#include "box.h"
#include "glm/gtx/euler_angles.hpp"
#include <iostream>

box::box() {
	rotmax = glm::vec3(10, 10, 10);
	rotmin = glm::vec3(-10, -10, -10);
	//strncpy(name, "name\0", 128);
}

void box::update(glm::mat4 world) {

	trans = glm::eulerAngleX(pose.x);
	trans = glm::eulerAngleY(pose.y) * trans;
	trans = glm::eulerAngleZ(pose.z) * trans;

	trans = glm::mat4(1.0, 0, 0, 0,
		0, 1.0, 0, 0,
		0, 0, 1.0, 0,
		offset[0], offset[1], offset[2], 1.0) * trans;

	trans = world * trans;

	for (box* child : childern) {
		child->update(trans);
	}
}

void drawWireBox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax) {
	glPushMatrix();
	glTranslatef(0.5f * (xmin + xmax), 0.5f * (ymin + ymax), 0.5f * (zmin + zmax));
	glScalef(xmax - xmin, ymax - ymin, zmax - zmin);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glutWireCube(1.0f);
	glPopMatrix();
}

void box::draw(glm::mat4 world) {

	trans = world * trans;

	GLfloat blurm[16];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			//std::cout << trans[i][j] << std::endl;
			blurm[(i * 4) + j] = trans[i][j];
		}
	}

	glLoadMatrixf(blurm);
	drawWireBox(min.x, min.y, min.z, max.x, max.y, max.z);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			//std::cout << trans[i][j] << std::endl;
			if (i == j) {
				blurm[(i * 4) + j] = 1.0;
			}
			else {
				blurm[(i * 4) + j] = 0.0;
			}
		}
	}
	glLoadMatrixf(blurm);

	for (box* child : childern) {
		//std::cout << "drawing child of  " << name << std::endl;
		child->draw(world);
	}
}

void box::adjustPose(float index, float dir) {
	std::cout << name << "  index  " << index << "direction  " << dir << std::endl;
	pose[index] += dir;
	if (pose[index] > rotmax[index]) {
		pose[index] = rotmax[index];
	}

	if (pose[index] < rotmin[index]) {
		pose[index] = rotmin[index];
	}

	if (pose[index] > 6.28 || pose[index] < -6.28) {
		pose[index] = 0;
	}
}

glm::mat4 box::getTransform()
{
	return trans;
}
