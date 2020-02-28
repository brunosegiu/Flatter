#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/SwapChain.h"

namespace Rendering {
namespace Vulkan {

class Framebuffer {
 public:
  Framebuffer(const DeviceRef device, const SwapChainRef swapChain,
              const VkFormat attachmentFormat);
  const VkFramebuffer& getNativeHandle() const { return mFramebufferHandle; };
  virtual ~Framebuffer();

 private:
  VkFramebuffer mFramebufferHandle;
  DeviceRef mDevice;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

}  // namespace Vulkan
}  // namespace Rendering
