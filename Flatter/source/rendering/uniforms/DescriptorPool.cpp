#include "rendering//uniforms/DescriptorPool.h"

#include <assert.h>

using namespace Rendering::Vulkan;

DescriptorPool::DescriptorPool(const ContextRef& context,
                               unsigned int uniformCount,
                               unsigned int textureCount,
                               unsigned int maxDescSets)
    : mContext(context) {
  std::vector<vk::DescriptorPoolSize> poolSize{};
  if (uniformCount > 0) {
    poolSize.push_back({vk::DescriptorType::eUniformBuffer, uniformCount});
  }
  if (textureCount > 0) {
    poolSize.push_back(
        {vk::DescriptorType::eCombinedImageSampler, textureCount});
  }

  auto const poolCreateInfo =
      vk::DescriptorPoolCreateInfo{}
          .setPoolSizeCount(static_cast<unsigned int>(poolSize.size()))
          .setPPoolSizes(poolSize.data())
          .setMaxSets(maxDescSets);

  assert(mContext->getDevice().createDescriptorPool(&poolCreateInfo, nullptr,
                                                    &mDescriptorPool) ==
         vk::Result::eSuccess);
}

DescriptorPool ::~DescriptorPool() {
  mContext->getDevice().destroyDescriptorPool(mDescriptorPool, nullptr);
}