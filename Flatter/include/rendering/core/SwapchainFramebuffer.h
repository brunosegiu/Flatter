#pragma once

#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//core/DepthBufferAttachment.h"
#include "rendering//renderpasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {
class RenderPass;
using RenderPassRef = std::shared_ptr<RenderPass>;

class SwapchainFramebuffer {
 public:
  SwapchainFramebuffer(const ContextRef& context,
                       const vk::Image& swapchainImage,
                       const vk::Format& format,
                       const vk::Extent2D& extent,
                       const RenderPassRef& renderPass,
                       const DepthBufferAttachmentRef& depthBuffer);
  const vk::Framebuffer& getHandle() const { return mFramebufferHandle; };
  virtual ~SwapchainFramebuffer();

 private:
  vk::ImageView mSwapchainImageView;
  vk::Framebuffer mFramebufferHandle;

  ContextRef mContext;
};

using SwapchainFramebufferRef = std::shared_ptr<SwapchainFramebuffer>;
}  // namespace Vulkan
}  // namespace Rendering
