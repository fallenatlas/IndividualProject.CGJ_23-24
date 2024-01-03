////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_CAMERA_HPP
#define MGL_CAMERA_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>

namespace mgl {

class Camera;

///////////////////////////////////////////////////////////////////////// Camera

class Camera {
 private:
  GLuint BindingPoint;
  GLuint UboId;
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;

  bool isActive;

 public:
  explicit Camera(GLuint bindingpoint, bool active);
  virtual ~Camera();
  void bindBuffer();
  void UnbindBuffer();
  glm::mat4 getViewMatrix();
  void updateViewMatrix(const glm::mat4& viewmatrix);
  void setViewMatrix();
  glm::mat4 getProjectionMatrix();
  void updateProjectionMatrix(const glm::mat4& projectionmatrix);
  void setProjectionMatrix();
  void setActive(bool status);
  bool getIsActive();
  GLint getBindingPoint();
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_CAMERA_HPP */
