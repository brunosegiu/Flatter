#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Framebuffer.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"

namespace Rendering {
namespace Vulkan {

class Framebuffer;
using FramebufferRef = std::shared_ptr<Framebuffer>;

class RenderPass {
 public:
  VkRenderPass mRenderPassHandle;
  RenderPass(const DeviceRef& device, const SurfaceRef& surface);

  void begin(const FramebufferRef& framebuffer,
             const SwapchainRef& swapchain,
             const VkCommandBuffer& command);

  virtual ~RenderPass();

 private:
  DeviceRef mDevice;
};

using RenderPassRef = std::shared_ptr<RenderPass>;

}  // namespace Vulkan
}  // namespace Rendering
