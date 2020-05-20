#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/FramebufferAttachment.h"
#include "rendering/vulkan/pipelines/Pipeline.h"

namespace Rendering {
namespace Vulkan {
class Framebuffer {
 public:
  Framebuffer(const ContextRef& context,
              const PipelineRef& pipeline,
              const std::vector<FramebufferAttachmentRef>& attachments,
              vk::Extent2D extent);

  const vk::Framebuffer& getHandle() const { return mFramebufferHandle; };

  virtual ~Framebuffer();

 private:
  Framebuffer(Framebuffer const&) = delete;
  Framebuffer& operator=(Framebuffer const&) = delete;

  ContextRef mContext;
  vk::Framebuffer mFramebufferHandle;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;
}  // namespace Vulkan
}  // namespace Rendering
