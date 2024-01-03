////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglCamera.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace mgl {

///////////////////////////////////////////////////////////////////////// Camera

Camera::Camera(GLuint bindingpoint, bool active)
    : ViewMatrix(glm::mat4(1.0f)), ProjectionMatrix(glm::mat4(1.0f)) {
    BindingPoint = bindingpoint;
    isActive = active;
    if (isActive) bindBuffer();
}

Camera::~Camera() {
    UnbindBuffer();
}

void Camera::bindBuffer() {
    glGenBuffers(1, &UboId);
    glBindBuffer(GL_UNIFORM_BUFFER, UboId);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, 0, GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, UboId);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    setViewMatrix();
    setProjectionMatrix();
}

void Camera::UnbindBuffer() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glDeleteBuffers(1, &UboId);
}

glm::mat4 Camera::getViewMatrix() { return ViewMatrix; }

void Camera::updateViewMatrix(const glm::mat4 &viewmatrix) {
  ViewMatrix = viewmatrix;
  if (isActive) setViewMatrix();
}

void Camera::setViewMatrix() {
    glBindBuffer(GL_UNIFORM_BUFFER, UboId);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
        glm::value_ptr(ViewMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 Camera::getProjectionMatrix() { return ProjectionMatrix; }

void Camera::updateProjectionMatrix(const glm::mat4 &projectionmatrix) {
  ProjectionMatrix = projectionmatrix;
  if (isActive) setProjectionMatrix();
}

void Camera::setProjectionMatrix() {
    glBindBuffer(GL_UNIFORM_BUFFER, UboId);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
        glm::value_ptr(ProjectionMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::setActive(bool status) {
    isActive = status;
    if (isActive) {
        bindBuffer();
    }
    else {
        UnbindBuffer();
    }
}

bool Camera::getIsActive() {
    return isActive;
}

GLint Camera::getBindingPoint() {
    return BindingPoint;
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
