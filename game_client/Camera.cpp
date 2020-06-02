////////////////////////////////////////
// Camera.cpp
////////////////////////////////////////

#include "Camera.h"

#include "glm/gtx/euler_angles.hpp"

////////////////////////////////////////////////////////////////////////////////

Camera::Camera(ClientParams& config)
	: config(config)
{
	Reset();
}

////////////////////////////////////////////////////////////////////////////////

void Camera::Update() {
	// Compute camera world matrix
	glm::mat4 world(1);
	world[3][2] = Distance;
	world = glm::eulerAngleY(glm::radians(-Azimuth)) * glm::eulerAngleX(glm::radians(-Incline)) * world;

	// Compute view matrix (inverse of world matrix)
	glm::mat4 view = glm::inverse(world);

	// Compute perspective projection matrix
	glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

	// Compute final view-projection matrix
	ViewProjectMtx = project * view;

}

////////////////////////////////////////////////////////////////////////////////

void Camera::Reset() {
	FOV = config.FOV;
	Aspect = config.aspect;
	NearClip = config.nearClip;
	FarClip = config.farClip;

	Distance = config.distance;
	Azimuth = config.azimuth;
	Incline = config.incline;
}

////////////////////////////////////////////////////////////////////////////////

glm::vec3 Camera::GetPosition() {
	return glm::eulerAngleY(glm::radians(-Azimuth)) 
		* glm::eulerAngleX(glm::radians(-Incline)) 
		* glm::translate(glm::vec3(0, 0, Distance))
		* glm::vec4(0, 0, 0, 1);
}

////////////////////////////////////////////////////////////////////////////////