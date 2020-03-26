#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <vector>

#include "input/InputEventListener.h"

namespace Input {

class InputController {
 public:
  InputController(SDL_Window* window,
                  const unsigned int winWidth,
                  const unsigned int winHeight);

  void attach(const InputEventListenerRef& listener);
  void processEvents(const float timeDelta) const;

  void wrapCursor(const unsigned int xCoords, const unsigned int yCoords);
  void wrapCursorToCenter();

  virtual ~InputController();

 private:
  std::vector<Input::InputEventListenerRef> mListeners;

  const SDL_Window* mWindow;
  const unsigned int mWinWidth;
  const unsigned int mWinHeight;
};

}  // namespace Input
