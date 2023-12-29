#include "./mglOrbitCamera.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace mgl {

///////////////////////////////////////////////////////////////////////// OrbitCamera

OrbitCamera::OrbitCamera(GLuint bindingpoint, bool active, glm::vec3 cameraPosition, glm::vec3 cameraFocusPoint, glm::vec3 cameraUp) : Camera(bindingpoint, active) {
    position = cameraPosition;
    focusPoint = cameraFocusPoint;
    up = cameraUp;
    zoom = glm::length(focusPoint - position);

	const glm::mat4 persp = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 40.0f);
	updateProjectionMatrix(persp);
}

OrbitCamera::~OrbitCamera() {}

void OrbitCamera::updateRotation(double elapsed) {
	glm::vec3 direction = glm::normalize(position - focusPoint);

	side = glm::normalize(glm::cross(direction, up));
	up = glm::cross(side, direction);
	
	//compute pitch quaternion
	glm::quat pitchQuat = glm::angleAxis(pitch * (float)elapsed, side);

	//compute yaw quaternion
	glm::quat yawQuat = glm::angleAxis(yaw * (float)elapsed, up);

	//compute total rotation
	glm::quat rotQuat = pitchQuat * yawQuat;

	position = glm::translate(focusPoint) * glm::scale(glm::vec3(zoom)) * glm::mat4(rotQuat) * glm::vec4(direction, 1.0f);

	//compute new view matrix
	glm::mat4 view = glm::lookAt(position, focusPoint, up);
	updateViewMatrix(view);

	//reset rotation angles
	pitch = 0.0f;
	yaw = 0.0f;
}

void OrbitCamera::addPitch(float degrees) {
	pitch += degrees;
}

void OrbitCamera::addYaw(float degrees) {
	yaw -= degrees;
}

void OrbitCamera::addZoom(float amount) {
	zoom += amount * 0.7f;
}

void OrbitCamera::updatePerspectiveProjectionMatrix(int winx, int winy) {
	ratio = (float)winx / (float)winy;

	if (perspectiveCameraActive) {
		setProjectionPerspective();
	}
	else {
		setProjectionOrthographic();
	}
}

void OrbitCamera::switchProjection() {
	if (perspectiveCameraActive) {
		setProjectionOrthographic();
		perspectiveCameraActive = false;
	}
	else {
		setProjectionPerspective();
		perspectiveCameraActive = true;
	}
}

void OrbitCamera::setProjectionPerspective() {
	const glm::mat4 persp = glm::perspective(glm::radians(30.0f), ratio, 1.0f, 40.0f);
	updateProjectionMatrix(persp);
}

void OrbitCamera::setProjectionOrthographic() {
	const glm::mat4 ortho = glm::ortho(-2.0f * ratio, 2.0f * ratio, -2.0f * 4.0f / 3.0f, 2.0f * 4.0f / 3.0f, 1.0f, 40.0f);
	updateProjectionMatrix(ortho);
}

glm::vec3 OrbitCamera::getUpVector() {
	return up;
}

glm::vec3 OrbitCamera::getSideVector() {
	return side;
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
