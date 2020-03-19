#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/RenderPass.h"

namespace Rendering {
namespace Vulkan {

class RenderPass;
using RenderPassRef = std::shared_ptr<RenderPass>;

class Framebuffer {
 public:
  Framebuffer(const DeviceRef& device,
              const VkImage& swapchainImage,
              const VkFormat& format,
              const VkExtent2D& extent,
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
