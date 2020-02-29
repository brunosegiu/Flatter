#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"

namespace Rendering {
namespace Vulkan {

class RenderPass;
using RenderPassRef = std::shared_ptr<RenderPass>;

class Framebuffer {
 public:
  Framebuffer(const VkImage& swapchainImage,
              const DeviceRef& device,
              const SurfaceRef& surface,
              const SwapchainRef& swapchain,
              const RenderPassRef& renderPass);
  const VkFramebuffer& getHandle() const { return mFramebufferHandle; };
  virtual ~Framebuffer();

 private:
  VkImageView mSwapchainImageViewHandle;
  VkFramebuffer mFramebufferHandle;

  DeviceRef mDevice;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

}  // namespace Vulkan
}  // namespace Rendering
