#include "rendering/lighting/LightController.h"

using namespace Rendering;

LightController::LightController(const Rendering::Vulkan::ContextRef& context,
                                 const unsigned int maxLightCount)
    : mContext(context), mMaxLightCount(maxLightCount) {
  mLights = std::vector<Light>(maxLightCount);
  mLights[0] =
      Light(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 50.0f));
  mLights[1] =
      Light(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec4(1.0f, 0.0f, 0.0f, 20.0f));
  mLights[2] =
      Light(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 20.0f));
  mDescriptorPool = std::make_shared<Rendering::Vulkan::DescriptorPool>(
      mContext, 1, 0, mMaxLightCount);
  mLayout = std::make_shared<DescriptorLayout>(
      mContext,
      std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>>{
          {vk::DescriptorType::eUniformBuffer,
           vk::ShaderStageFlagBits::eFragment},
      });
  mUniform = std::make_shared<Rendering::Vulkan::Uniform<std::vector<Light>>>(
      mContext, mLayout, mDescriptorPool, sUniformBinding, mLights,
      mLights.size() * sizeof(Light));
}

void LightController::add(const Light& light) {
  mLights.emplace_back(light);
  mUniform->update(mLights);
}