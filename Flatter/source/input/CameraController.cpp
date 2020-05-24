#include "input/CameraController.h"

using namespace Input;

CameraController::CameraController()
    : InputEventListener(),
      mCamera(),
      mSpeed(0.00001f),
      mIsFirstUpdate(true),
      mIsEnabled(false) {}

void CameraController::onKeyPress(const float timeDelta,
                                  const SDL_Scancode key) {
  if (!mIsEnabled)
    return;
  const float factor = mSpeed * timeDelta;
  switch (key) {
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP:
      mCamera.moveForwards(factor);
      break;
    case SDL_SCANCODE_A:
    case SDL_SCANCODE_LEFT:
      mCamera.moveLeft(factor);
      break;
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
      mCamera.moveBackwards(factor);
      break;
    case SDL_SCANCODE_D:
    case SDL_SCANCODE_RIGHT:
      mCamera.moveRight(factor);
      break;
    default:
      break;
  }
}

void CameraController::onCursorMovement(const float timeDelta,
                                        const glm::vec2& position) {
  if (!mIsEnabled)
    return;
  if (mIsFirstUpdate) {
    mIsFirstUpdate = false;
  } else {
    mCamera.rotate(position.x, position.y);
  }
}

void CameraController::onLeftClick(const float timeDelta) {
  mIsEnabled = !mIsEnabled;
}

CameraController::~CameraController() {}