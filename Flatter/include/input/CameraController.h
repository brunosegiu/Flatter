#pragma once

#include <SDL2/SDL.h>

#include "rendering/Camera.h"

namespace Input {
class CameraController {
 public:
  CameraController();

  void process(const float& delta);

  const Rendering::Camera& getCamera() const { return mCamera; };

  virtual ~CameraController();

 private:
  Rendering::Camera mCamera;

  float mSpeed;
};
}  // namespace Input
