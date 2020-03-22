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

  glm::vec2 mPrevMouseCoords;

  float mSpeed;

  const float mWidth, mHeight;

  void updateMovement(const float& timeDelt);
  void updateRotation(const float& timeDelt);
};
}  // namespace Input
