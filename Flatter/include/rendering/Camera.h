#pragma once

#include <glm/glm.hpp>

namespace Rendering {
class Camera {
 public:
  Camera();

  const glm::mat4& getViewProjection() const { return mVP; };

  void rotate(const float& angle, const glm::vec3& axis = glm::vec3(0, 1, 0));
  void rotate(const float& yaw, const float& pitch);

  void translate(const glm::vec3& value);
  void translateTo(const glm::vec3& value);
  void scale(const glm::vec3& value);

  void moveRight(const float& value);
  void moveLeft(const float& value);
  void moveForwards(const float& value);
  void moveBackwards(const float& value);

  virtual ~Camera();

 private:
  void updateViewProjection();

  glm::vec3 mEye;
  glm::vec3 mDir;
  glm::vec3 mUp;

  glm::mat4 mProjection;
  glm::mat4 mView;
  glm::mat4 mVP;
};
}  // namespace Rendering
