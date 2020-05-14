#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <memory>

#include "input/InputEventListener.h"

namespace Input {
class InputEventListener {
 public:
  InputEventListener(){};

  virtual void onKeyPress(const float timeDelta, const SDL_Scancode key){};
  virtual void onCursorMovement(const float timeDelta,
                                const glm::vec2& position){};
  virtual void onMouseClick(const float timeDelta){};
  virtual void onQuit(){};

  virtual ~InputEventListener(){};
};

using InputEventListenerRef = std::shared_ptr<InputEventListener>;
}  // namespace Input
