#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"

namespace Rendering {
namespace Vulkan {
class DescriptorLayout {
 public:
  DescriptorLayout(
      const ContextRef& context,
      const std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>>&
          descriptorLayouts);

  const vk::DescriptorSetLayout& getHandle() const {
    return mDescriptorSetLayout;
  };

  virtual ~DescriptorLayout();

 private:
  DescriptorLayout(DescriptorLayout const&) = delete;
  DescriptorLayout& operator=(DescriptorLayout const&) = delete;

  ContextRef mContext;
  vk::DescriptorSetLayout mDescriptorSetLayout;
};

using DescriptorLayoutRef = std::shared_ptr<DescriptorLayout>;
}  // namespace Vulkan
}  // namespace Rendering
