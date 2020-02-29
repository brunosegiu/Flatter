#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"

namespace Rendering {
namespace Vulkan {

class Framebuffer {
 public:
  VkImageView mSwapchainImageViewHandle;
  VkFramebuffer mFramebufferHandle;

  Framebuffer(const VkImage& swapchainImage,
              const Device& device,
              Surface& surface,
              const Swapchain& swapchain,
              const RenderPass& renderPass);

  virtual ~Framebuffer();
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

}  // namespace Vulkan
}  // namespace Rendering
