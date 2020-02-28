#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class RenderPass {
 public:
  RenderPass(const DeviceRef device, const VkFormat attachmentFormat);
  const VkRenderPass& getNativeHandle() { return mRenderPassHandle; }
  void begin(const VkCommandBuffer& commandBufferHandle,
             const FramebufferRef framebuffer);
  void end(const VkCommandBuffer& commandBufferHandle);
  virtual ~RenderPass();

 private:
  VkRenderPass mRenderPassHandle;
  DeviceRef mDevice;
};

using RenderPassRef = std::shared_ptr<RenderPass>;

}  // namespace Vulkan
}  // namespace Rendering
