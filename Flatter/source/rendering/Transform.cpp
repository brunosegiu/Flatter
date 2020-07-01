#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

Transform::Transform()
    : mMatrix(glm::mat4(1.0f)),
      mCurrentTranslation(glm::vec3(0.0f)),
      mCurrentRotation(0.0f),
      mCurrentScale(glm::vec3(1.0f)) {}

void Transform::setPosition(const glm::vec3& value) {
  mCurrentTranslation = value;
  updateMatrix();
}
void Transform::setRotation(const glm::vec3& value) {
  mCurrentRotation = value;
  updateMatrix();
}
void Transform::setScale(const glm::vec3& value) {
  mCurrentScale = value;
  updateMatrix();
}

void Transform::rotate(const glm::vec3& value) {
  mCurrentRotation += value;
  updateMatrix();
}

void Transform::translate(const glm::vec3& value) {
  mCurrentTranslation += value;
  updateMatrix();
}

void Transform::scale(const glm::vec3& value) {
  mCurrentScale += value;
  updateMatrix();
}

void Transform::updateMatrix() {
  mMatrix = glm::translate(glm::mat4(1.0f), mCurrentTranslation) *
            glm::scale(glm::mat4(1.0f), mCurrentScale) *
            glm::eulerAngleYXZ(mCurrentRotation.y, mCurrentRotation.x,
                               mCurrentRotation.z);
}

Transform::~Transform() {}