#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "rendering//core/Context.h"
#include "rendering//uniforms/DescriptorLayout.h"
#include "rendering//uniforms/DescriptorPool.h"
#include "rendering//uniforms/Uniform.h"

struct Light {
  alignas(16) glm::vec3 mPosition;
  alignas(16) glm::vec4 mColor;

  Light(glm::vec3 position = glm::vec3(0.0f), glm::vec4 color = glm::vec4(1.0f))
      : mPosition(position), mColor(color){};
};

namespace Rendering {
using LightUniformRef =
    std::shared_ptr<Rendering::Vulkan::Uniform<std::vector<Light>>>;
class LightController {
 public:
  LightController(const Rendering::Vulkan::ContextRef& context,
                  const unsigned int maxLightCount = 256);

  const LightUniformRef& getUniform() const { return mUniform; };
  const DescriptorLayoutRef& getLayout() const { return mLayout; };

  void add(const Light& light);

 private:
  LightController(LightController const&) = delete;
  LightController& operator=(LightController const&) = delete;

  static const unsigned int sUniformBinding = 0;
  static const unsigned int sUniformSet = 1;

  std::vector<Light> mLights;
  const unsigned int mMaxLightCount;

  Rendering::Vulkan::ContextRef mContext;
  Rendering::Vulkan::DescriptorPoolRef mDescriptorPool;
  DescriptorLayoutRef mLayout;
  LightUniformRef mUniform;
};

using LightControllerRef = std::shared_ptr<LightController>;
}  // namespace Rendering
