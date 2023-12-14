////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_ORBIT_CAMERA_HPP
#define MGL_ORBIT_CAMERA_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "./mglCamera.hpp"

namespace mgl {

class OrbitCamera;

///////////////////////////////////////////////////////////////////////// Camera

class OrbitCamera : public Camera {
 private:
	 glm::vec3 up;

	 glm::vec3 position;
	 glm::vec3 focusPoint;
	 float zoom = 0.0f;

	 float pitch = 0.0f;
	 float yaw = 0.0f;

	 float ratio = 4.0f / 3.0f;
	 bool perspectiveCameraActive = true;

 public:
  explicit OrbitCamera(GLuint bindingpoint, bool active, glm::vec3 cameraPosition, glm::vec3 focusPoint, glm::vec3 up);
  virtual ~OrbitCamera();
  void addPitch(float degrees);
  void addYaw(float degrees);
  void addZoom(float amount);
  void updateRotation(double elapsed);
  void updatePerspectiveProjectionMatrix(int winx, int winy);
  void switchProjection();
  void setProjectionPerspective();
  void setProjectionOrthographic();
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_ORBIT_CAMERA_HPP */
