#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {
class DescriptorPool {
 public:
  DescriptorPool(const ContextRef& context,
                 unsigned int uniformCount,
                 unsigned int textureCount,
                 unsigned int maxDescSets);

  const vk::DescriptorPool& getDescriptorPool() const {
    return mDescriptorPool;
  };

  virtual ~DescriptorPool();

 private:
  DescriptorPool(DescriptorPool const&) = delete;
  DescriptorPool& operator=(DescriptorPool const&) = delete;

  vk::DescriptorPool mDescriptorPool;

  ContextRef mContext;
};

using DescriptorPoolRef = std::shared_ptr<DescriptorPool>;
}  // namespace Vulkan
}  // namespace Rendering
