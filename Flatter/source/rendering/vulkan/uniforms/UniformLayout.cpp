#include "rendering/vulkan/uniforms/UniformLayout.h"

#include <algorithm>

using namespace Rendering::Vulkan;

UniformLayout::UniformLayout(
    const ContextRef& context,
    const std::vector<vk::DescriptorType>& uniformTypes,
    vk::ShaderStageFlags stages)
    : mContext(context) {
  std::vector<vk::DescriptorSetLayoutBinding> uboLayoutBindings;
  uboLayoutBindings.reserve(uniformTypes.size());
  for (unsigned int unifIndex = 0; unifIndex < uniformTypes.size();
       ++unifIndex) {
    uboLayoutBindings.push_back(vk::DescriptorSetLayoutBinding{}
                                    .setBinding(unifIndex)
                                    .setDescriptorType(uniformTypes[unifIndex])
                                    .setDescriptorCount(1)
                                    .setStageFlags(stages));
  }
  auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo{}
                              .setBindingCount(uniformTypes.size())
                              .setPBindings(uboLayoutBindings.data());

  mContext->getDevice().createDescriptorSetLayout(&layoutInfo, nullptr,
                                                  &mDescriptorSetLayout);
}

UniformLayout::~UniformLayout() {
  mContext->getDevice().destroyDescriptorSetLayout(mDescriptorSetLayout);
}