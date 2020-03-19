#pragma once

#include <glm/glm.hpp>

namespace Rendering {
class Camera {
 public:
  Camera();

  const glm::mat4& getViewProjection() const { return mVP; };

  virtual ~Camera();

 private:
  void updateViewProjection();

  glm::mat4 mProjection;
  glm::mat4 mView;

  glm::mat4 mVP;
};
}  // namespace Rendering
