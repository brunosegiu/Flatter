﻿#include "input/CameraController.h"

using namespace Input;

CameraController::CameraController(const unsigned int& width,
                                   const unsigned int& height)
    : mCamera(),
      mSpeed(0.00001f),
      mWidth(static_cast<float>(width)),
      mHeight(static_cast<float>(height)) {}

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
  SDL_GetMouseState(&mouseState.x, &mouseState.y);
  const glm::vec2 currentPosNorm(static_cast<float>(mouseState.x) / mWidth,
                                 static_cast<float>(mouseState.y) / mHeight);
  const glm::vec2 movementDir = mPrevMouseCoords - currentPosNorm;
  mPrevMouseCoords = currentPosNorm;

  mCamera.rotateUp(movementDir.x);
  mCamera.rotateRight(movementDir.y);
}

CameraController::~CameraController() {}