#include "rendering//uniforms/DescriptorLayout.h"

#include <algorithm>

using namespace Rendering::Vulkan;

DescriptorLayout::DescriptorLayout(
    const ContextRef& context,
    const std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>>&
        descriptorLayouts)
    : mContext(context) {
  std::vector<vk::DescriptorSetLayoutBinding> uboLayoutBindings;
  uboLayoutBindings.reserve(descriptorLayouts.size());
  for (unsigned int unifIndex = 0; unifIndex < descriptorLayouts.size();
       ++unifIndex) {
    uboLayoutBindings.push_back(
        vk::DescriptorSetLayoutBinding{}
            .setBinding(unifIndex)
            .setDescriptorType(descriptorLayouts[unifIndex].first)
            .setDescriptorCount(1)
            .setStageFlags(descriptorLayouts[unifIndex].second));
  }
  auto const layoutInfo =
      vk::DescriptorSetLayoutCreateInfo{}
          .setBindingCount(static_cast<unsigned int>(uboLayoutBindings.size()))
          .setPBindings(uboLayoutBindings.data());

  mContext->getDevice().createDescriptorSetLayout(&layoutInfo, nullptr,
                                                  &mDescriptorSetLayout);
}

DescriptorLayout::~DescriptorLayout() {
  mContext->getDevice().destroyDescriptorSetLayout(mDescriptorSetLayout);
}