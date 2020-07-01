#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//core/FramebufferAttachment.h"

namespace Rendering {
namespace Vulkan {
class DepthBufferAttachment : public FramebufferAttachment {
 public:
  static const vk::Format pickFormat(const ContextRef& context);

  DepthBufferAttachment(const ContextRef& context, const vk::Extent2D& extent);

  virtual ~DepthBufferAttachment();

 private:
  DepthBufferAttachment(DepthBufferAttachment const&) = delete;
  DepthBufferAttachment& operator=(DepthBufferAttachment const&) = delete;
};

using DepthBufferAttachmentRef = std::shared_ptr<DepthBufferAttachment>;
}  // namespace Vulkan
}  // namespace Rendering
#pragma once
