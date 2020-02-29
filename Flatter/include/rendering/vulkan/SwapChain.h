#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Framebuffer.h"
#include "rendering/vulkan/Surface.h"

namespace Rendering {
namespace Vulkan {

class Swapchain {
 public:
  Swapchain(const DeviceRef &device, const Rendering::Vulkan::Surface &surface,
            unsigned int frameCount, const unsigned int width,
            const unsigned int height);
  const VkSwapchainKHR &getNativeHandle() const { return mSwapChainHandle; };
  void acquireImage(const unsigned int &currentFrameIndex);
  const VkSemaphore &getImageSemaphore(
      const unsigned int &currentFrameIndex) const {
    return mImageSemaphores[currentFrameIndex];
  };
  virtual ~Swapchain();

 private:
  VkSwapchainKHR mSwapChainHandle;
  DeviceRef mDevice;
  std::vector<VkImage> mSwapChainImages;
  std::vector<VkSemaphore> mImageSemaphores;
  VkExtent2D mExtent;
  std::vector<FramebufferRef> mFramebuffers;

  const VkPresentModeKHR getPresentMode(
      const std::vector<VkPresentModeKHR> &presentModes) const;
  Swapchain(const Swapchain &) = delete;
};

using SwapchainRef = std::shared_ptr<Swapchain>;

}  // namespace Vulkan
}  // namespace Rendering
