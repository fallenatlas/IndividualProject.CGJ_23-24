////////////////////////////////////////////////////////////////////////////////
//
// Application Class
//
// Copyright (c)2022 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_KEY_STATE_HPP
#define MGL_KEY_STATE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace mgl {

class KeyState;

///////////////////////////////////////////////////////////////////////// Engine

class KeyState {
 public:
  static KeyState &getInstance();

  bool isKeyPressed(int key);
  void updateKeyState(int key, int action);
  bool isMouseButtonPressed(int button);
  void updateMouseButtonState(int button, int action);

 protected:
  virtual ~KeyState();

 private:
  bool keys[GLFW_KEY_LAST] = { false };
  bool mouseButtons[GLFW_MOUSE_BUTTON_LAST] = { false };

  KeyState();

 public:
  KeyState(KeyState const &) = delete;
  void operator=(KeyState const &) = delete;
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_APP_HPP */
