#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"

namespace Rendering {
namespace Vulkan {

class RenderPass;

class Framebuffer {
 public:
  Framebuffer(const VkImage& swapchainImage,
              const DeviceRef& device,
              Surface& surface,
              const Swapchain& swapchain,
              const RenderPass& renderPass);
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
