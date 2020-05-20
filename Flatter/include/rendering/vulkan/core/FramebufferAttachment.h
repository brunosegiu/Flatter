#pragma once

#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {
class FramebufferAttachment {
 public:
  FramebufferAttachment(
      const ContextRef& context,
      const vk::Extent2D& extent,
      const vk::Format& format,
      const vk::ImageUsageFlags& usage,
      const vk::ImageAspectFlags& viewAspect,
      const vk::ImageTiling tiling = vk::ImageTiling::eOptimal);

  const vk::ImageView& getImageView() const { return mView; };
  const vk::Format& getFormat() const { return mFormat; };

  virtual ~FramebufferAttachment();

 protected:
  vk::Image mImage;
  vk::ImageView mView;
  vk::DeviceMemory mMemory;
  vk::Format mFormat;

  ContextRef mContext;

 private:
  FramebufferAttachment(FramebufferAttachment const&) = delete;
  FramebufferAttachment& operator=(FramebufferAttachment const&) = delete;
};

using FramebufferAttachmentRef = std::shared_ptr<FramebufferAttachment>;
}  // namespace Vulkan
}  // namespace Rendering
