#pragma once

#include "input/InputEventListener.h"
#include "rendering/Camera.h"

namespace Input {
class CameraController : public InputEventListener {
 public:
  CameraController();

  void onKeyPress(const float timeDelta, const SDL_Scancode key) override;
  void onCursorMovement(const float timeDelta,
                        const glm::vec2& position) override;
  void onMouseClick(const float timeDelta) override;

  Rendering::Camera& getCamera() { return mCamera; };

  virtual ~CameraController();

 private:
  Rendering::Camera mCamera;
  float mSpeed;
  bool mIsFirstUpdate;
};

using CameraControllerRef = std::shared_ptr<CameraController>;
}  // namespace Input
