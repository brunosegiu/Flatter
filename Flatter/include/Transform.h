#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <vector>

class Transform {
 public:
  Transform();

  void setPosition(const glm::vec3& value);
  void setScale(const glm::vec3& value);
  void setRotation(const glm::vec3& value);

  void translate(const glm::vec3& value);
  void scale(const glm::vec3& value);
  void rotate(const glm::vec3& value);

  const glm::mat4& getMatrix() const { return mMatrix; };

  const glm::vec3 getScale() const { return mCurrentScale; };
  const glm::vec3 getTranslation() const { return mCurrentTranslation; };

  virtual ~Transform();

 private:
  Transform(Transform const&) = delete;
  Transform& operator=(Transform const&) = delete;

  void updateMatrix();

  glm::mat4 mMatrix;
  glm::vec3 mCurrentTranslation, mCurrentScale, mCurrentRotation;
};

using TransformRef = std::shared_ptr<Transform>;
