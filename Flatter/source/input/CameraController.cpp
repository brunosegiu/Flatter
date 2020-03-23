#include "input/CameraController.h"

using namespace Input;

CameraController::CameraController(const unsigned int& width,
                                   const unsigned int& height)
    : mCamera(),
      mSpeed(0.00001f),
      mWidth(static_cast<float>(width)),
      mHeight(static_cast<float>(height)),
      mIsFirstUpdate(true) {}

void CameraController::process(const float& timeDelta) {
  updateMovement(timeDelta);
  updateRotation(timeDelta);
}

void CameraController::updateMovement(const float& timeDelta) {
  const float factor = mSpeed * timeDelta;
  const auto pressedKeys = SDL_GetKeyboardState(nullptr);

  if (pressedKeys[SDL_SCANCODE_W] || pressedKeys[SDL_SCANCODE_UP]) {
    mCamera.moveForwards(factor);
  }
  if (pressedKeys[SDL_SCANCODE_A] || pressedKeys[SDL_SCANCODE_LEFT]) {
    mCamera.moveLeft(factor);
  }
  if (pressedKeys[SDL_SCANCODE_S] || pressedKeys[SDL_SCANCODE_DOWN]) {
    mCamera.moveBackwards(factor);
  }
  if (pressedKeys[SDL_SCANCODE_D] || pressedKeys[SDL_SCANCODE_RIGHT]) {
    mCamera.moveRight(factor);
  }
}

void CameraController::updateRotation(const float& timeDelta) {
  glm::ivec2 mouseState;
  if (mIsFirstUpdate) {
    mIsFirstUpdate = false;
  } else {
    SDL_GetMouseState(&mouseState.x, &mouseState.y);
    const glm::vec2 movementDir(
        (-static_cast<float>(mouseState.x) / mWidth) + 0.5f,
        (-static_cast<float>(mouseState.y) / mHeight) + 0.5f);
    mCamera.rotate(movementDir.x, movementDir.y);
  }
}

CameraController::~CameraController() {}
