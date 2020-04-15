﻿#include "rendering/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace Rendering;

Camera::Camera() {
  const glm::mat4 clipMatrix =
      glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);
  mProjection =
      clipMatrix * glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);

  mEye = glm::vec3(0, 0, 10.0f);
  mDir = glm::vec3(0, 0, 1.0f);
  mUp = glm::vec3(0, 1, 0);

  updateViewProjection();
}

void Camera::updateViewProjection() {
  mView = glm::lookAt(mEye, mEye + mDir * glm::length(mEye), mUp);
  mVP = mProjection * mView;
}

void Camera::rotate(const float& angle, const glm::vec3& axis) {
  mDir = glm::mat3(glm::rotate(angle, axis)) * mDir;
}

void Camera::translate(const glm::vec3& value) {
  mEye += value;
}

void Camera::translateTo(const glm::vec3& value) {
  mEye = value;
}

void Camera::moveRight(const float& value) {
  const glm::vec3 right = glm::cross(mDir, mUp);
  translate(right * value);
}

void Camera::moveLeft(const float& value) {
  const glm::vec3 left = -glm::cross(mDir, mUp);
  translate(left * value);
}

void Camera::moveForwards(const float& value) {
  translate(mDir * value);
}

void Camera::moveBackwards(const float& value) {
  translate(-mDir * value);
}

void Camera::rotate(const float& yaw, const float& pitch) {
  const glm::vec3 right = glm::cross(mDir, mUp);
  rotate(yaw, mUp);
  rotate(pitch, right);
}

Camera::~Camera() {}
