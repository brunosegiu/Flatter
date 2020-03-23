#pragma once

#include <SDL2/SDL.h>

#include "rendering/Camera.h"

namespace Input {
class CameraController {
 public:
  CameraController(const unsigned int& width, const unsigned int& height);

  void process(const float& timeDelta);

  const Rendering::Camera& getCamera() const { return mCamera; };

  virtual ~CameraController();

 private:
  Rendering::Camera mCamera;

  float mSpeed;

  const float mWidth, mHeight;
  bool mIsFirstUpdate;

  void updateMovement(const float& timeDelta);
  void updateRotation(const float& timeDelta);
};
}  // namespace Input
