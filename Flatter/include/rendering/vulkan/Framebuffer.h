#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class Framebuffer {
 public:
  Framebuffer(const DeviceRef device, const VkImage& image,
              const VkExtent2D& extent, const VkFormat attachmentFormat);
  const VkFramebuffer& getNativeHandle() const { return mFramebufferHandle; };
  virtual ~Framebuffer();

 private:
  VkFramebuffer mFramebufferHandle;
  VkImageView mImageViewHandle;

  DeviceRef mDevice;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

}  // namespace Vulkan
}  // namespace Rendering
