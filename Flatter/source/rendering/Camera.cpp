#include "rendering/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace Rendering;

Camera::Camera() {
  const glm::mat4 clipMatrix =
      glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);
  mProjection =
      clipMatrix * glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

  mView =
      glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  updateViewProjection();
}

void Camera::updateViewProjection() {
  mVP = mProjection * mView;
}

Camera::~Camera() {}
