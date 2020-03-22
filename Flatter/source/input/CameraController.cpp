#include "input/CameraController.h"

using namespace Input;

CameraController::CameraController() : mCamera(), mSpeed(0.00001f) {}

void CameraController::process(const float& delta) {
  const float factor = mSpeed * delta;
  const Uint8* state = SDL_GetKeyboardState(NULL);

  if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
    mCamera.moveForwards(factor);
  }
  if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
    mCamera.moveLeft(factor);
  }
  if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
    mCamera.moveBackwards(factor);
  }
  if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
    mCamera.moveRight(factor);
  }
}

CameraController::~CameraController() {}
