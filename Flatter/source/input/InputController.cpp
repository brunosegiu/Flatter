#include "input/InputController.h"

using namespace Input;

InputController::InputController(SDL_Window* window,
                                 const unsigned int winWidth,
                                 const unsigned int winHeight)
    : mWindow(window), mWinWidth(winWidth), mWinHeight(winHeight) {}

void InputController::attach(const InputEventListenerRef& listener) {
  mListeners.push_back(listener);
}

void InputController::processEvents(const float timeDelta) const {
  SDL_Event evt;
  while (SDL_PollEvent(&evt)) {
    const bool shouldQuit =
        !(evt.type == SDL_QUIT ||
          (evt.type == SDL_KEYDOWN && (evt.key.keysym.sym == SDLK_ESCAPE)));

    if (shouldQuit) {
      for (const auto& listener : mListeners) {
        listener->onQuit();
      }
    }
    if (evt.type == SDL_KEYDOWN) {
      for (const auto& listener : mListeners) {
        listener->onKeyPressed(evt.key.keysym.sym);
      }
    }
  }
}

void InputController::wrapCursor(const unsigned int xCoords,
                                 const unsigned int yCoords) {
  SDL_WarpMouseInWindow(mWindow, xCoords, yCoords);
}

void InputController::wrapCursorToCenter() {
  wrapCursor(mWinWidth / 2, mWinHeight / 2);
}

InputController::~InputController() {}
