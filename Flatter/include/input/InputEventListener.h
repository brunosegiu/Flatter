#pragma once

#include <SDL2/SDL.h>

#include "input/InputEventListener.h"

namespace Input {

class InputEventListener {
 public:
  InputEventListener(){};

  virtual void onKeyPress(const float timeDelta, const SDL_Keycode key){};
  virtual void onCursorMovement(const float timeDelta){};
  virtual void onMouseClick(const float timeDelta){};
  virtual void onQuit(const float timeDelta){};

  virtual ~InputEventListener(){};
};

using InputEventListenerRef = std::shared_ptr<InputEventListenerRef>;

}  // namespace Input
