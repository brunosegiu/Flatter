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

class RenderPass {
 public:
  VkRenderPass mRenderPassHandle;
  RenderPass(const DeviceRef& device, Surface& surface);

  void begin(const Framebuffer& framebuffer,
             const Swapchain& swapchain,
             const VkCommandBuffer& command);

  virtual ~RenderPass();

 private:
  DeviceRef mDevice;
};

}  // namespace Vulkan
}  // namespace Rendering
