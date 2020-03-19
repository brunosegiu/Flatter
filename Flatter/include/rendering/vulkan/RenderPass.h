#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class RenderPass {
 public:
  RenderPass(const DeviceRef& device, const VkFormat& surfaceFormat);

  const VkRenderPass& getHandle() const { return mRenderPassHandle; };

  virtual ~RenderPass();

 private:
  VkRenderPass mRenderPassHandle;

  DeviceRef mDevice;
};

using RenderPassRef = std::shared_ptr<RenderPass>;

}  // namespace Vulkan
}  // namespace Rendering
