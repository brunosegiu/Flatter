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
  void onLeftClick(const float timeDelta) override;

  Rendering::Camera& getCamera() { return mCamera; };

  virtual ~CameraController();

 private:
  Rendering::Camera mCamera;
  float mSpeed;
  bool mIsFirstUpdate;
  bool mIsEnabled;
};

using CameraControllerRef = std::shared_ptr<CameraController>;
}  // namespace Input
