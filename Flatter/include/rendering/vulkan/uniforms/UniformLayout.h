#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {
class UniformLayout {
 public:
  UniformLayout(
      const ContextRef& context,
      const std::vector<vk::DescriptorType>& uniformTypes,
      vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAllGraphics);

  const vk::DescriptorSetLayout& getHandle() const {
    return mDescriptorSetLayout;
  };

  virtual ~UniformLayout();

 private:
  UniformLayout(UniformLayout const&) = delete;
  UniformLayout& operator=(UniformLayout const&) = delete;

  ContextRef mContext;
  vk::DescriptorSetLayout mDescriptorSetLayout;
};

using UniformLayoutRef = std::shared_ptr<UniformLayout>;
}  // namespace Vulkan
}  // namespace Rendering
