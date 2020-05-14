#pragma once

#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/DepthBufferAttachment.h"
#include "rendering/vulkan/renderpasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {
class RenderPass;
using RenderPassRef = std::shared_ptr<RenderPass>;

class Framebuffer {
 public:
  Framebuffer(const ContextRef& context,
              const vk::Image& swapchainImage,
              const vk::Format& format,
              const vk::Extent2D& extent,
              const RenderPassRef& renderPass,
              const DepthBufferAttachmentRef& depthBuffer);
  const vk::Framebuffer& getHandle() const { return mFramebufferHandle; };
  virtual ~Framebuffer();

 private:
  vk::ImageView mSwapchainImageView;
  vk::Framebuffer mFramebufferHandle;

  ContextRef mContext;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;
}  // namespace Vulkan
}  // namespace Rendering
