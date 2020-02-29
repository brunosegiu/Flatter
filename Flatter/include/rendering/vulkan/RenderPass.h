#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Surface.h"

namespace Rendering {
namespace Vulkan {

class RenderPass {
 public:
  VkRenderPass mRenderPassHandle;

  RenderPass(Surface& surface, const Device& device);

  virtual ~RenderPass();
};

}  // namespace Vulkan
}  // namespace Rendering
