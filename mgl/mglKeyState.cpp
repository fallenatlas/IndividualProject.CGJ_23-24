////////////////////////////////////////////////////////////////////////////////
//
// Application Class
//
// Copyright (c)2022-23 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglKeyState.hpp"

#include <iostream>

#include "./mglError.hpp"

namespace mgl {

////////////////////////////////////////////////////////////////////////// SETUP

KeyState::KeyState(void) {}

KeyState::~KeyState(void) {}

KeyState &KeyState::getInstance(void) {
  static KeyState instance;
  return instance;
}

bool KeyState::isKeyPressed(int key) {
    return keys[key];
}

void KeyState::updateKeyState(int key, int action) {
    if (key == GLFW_KEY_UNKNOWN) return;
    if (action == GLFW_PRESS) {
        keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

bool KeyState::isMouseButtonPressed(int button) {
    return mouseButtons[button];
}

void KeyState::updateMouseButtonState(int key, int action) {
    if (key == GLFW_KEY_UNKNOWN) return;
    if (action == GLFW_PRESS) {
        mouseButtons[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        mouseButtons[key] = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
