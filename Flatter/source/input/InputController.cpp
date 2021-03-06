﻿#include "input/InputController.h"

using namespace Input;

InputController::InputController(SDL_Window* window,
                                 const unsigned int winWidth,
                                 const unsigned int winHeight)
    : mWindow(window),
      mWinWidth(winWidth),
      mWinHeight(winHeight),
      mShouldWrapCursor(false) {}

void InputController::attach(const InputEventListenerRef& listener) {
  mListeners.push_back(listener);
}

void InputController::processEvents(const float timeDelta) {
  glm::ivec2 mouseState;
  SDL_GetMouseState(&mouseState.x, &mouseState.y);
  const glm::vec2 mousePosNorm(
      (0.5f - static_cast<float>(mouseState.x) / mWinWidth),
      (0.5f - static_cast<float>(mouseState.y) / mWinHeight));
  for (const auto& listener : mListeners) {
    listener->onCursorMovement(timeDelta, mousePosNorm);
  }

  SDL_Event evt;
  while (SDL_PollEvent(&evt)) {
    const bool shouldQuit =
        (evt.type == SDL_QUIT ||
         (evt.type == SDL_KEYDOWN && (evt.key.keysym.sym == SDLK_ESCAPE)));

    if (shouldQuit) {
      for (const auto& listener : mListeners) {
        listener->onQuit();
      }
    }

    if (evt.type == SDL_MOUSEBUTTONDOWN) {
      if (evt.button.button == SDL_BUTTON_LEFT) {
        mShouldWrapCursor = !mShouldWrapCursor;
        for (const auto& listener : mListeners) {
          listener->onLeftClick(timeDelta);
        }
      }
      if (evt.button.button == SDL_BUTTON_RIGHT) {
        for (const auto& listener : mListeners) {
          listener->onRightClick(timeDelta);
        }
      }
    }
  }

  int keyCount = 0;
  const Uint8* pressedKeys = SDL_GetKeyboardState(&keyCount);
  for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex) {
    if (pressedKeys[keyIndex]) {
      for (const auto& listener : mListeners) {
        listener->onKeyPress(timeDelta, static_cast<SDL_Scancode>(keyIndex));
      }
    }
  }
}

void InputController::wrapCursor(const unsigned int xCoords,
                                 const unsigned int yCoords) {
  if (mShouldWrapCursor)
    SDL_WarpMouseInWindow(mWindow, xCoords, yCoords);
}

void InputController::wrapCursorToCenter() {
  wrapCursor(mWinWidth / 2, mWinHeight / 2);
}

InputController::~InputController() {}