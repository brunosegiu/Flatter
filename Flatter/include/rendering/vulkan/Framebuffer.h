#pragma once

#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/Context.h"
#include "rendering/vulkan/RenderPass.h"

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
              const RenderPassRef& renderPass);
  const vk::Framebuffer& getHandle() const { return mFramebufferHandle; };
  virtual ~Framebuffer();

 private:
  vk::ImageView mSwapchainImageViewHandle;
  vk::Framebuffer mFramebufferHandle;

  ContextRef mContext;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

}  // namespace Vulkan
}  // namespace Rendering
