#pragma once

#include <rendering/vulkan/Device.h>
#include <rendering/vulkan/Surface.h>
#include <vulkan/vulkan.h>

namespace Rendering {
namespace Vulkan {

class SwapChain {
 public:
  SwapChain(const DeviceRef &device, const Rendering::Vulkan::Surface &surface,
            unsigned int frameCount, const unsigned int width,
            const unsigned int height);
  const VkSwapchainKHR &getNativeHandle() const { return mSwapChainHandle; };
  void acquireImage(const unsigned int &currentFrameIndex);
  const VkSemaphore &getImageSemaphore(
      const unsigned int &currentFrameIndex) const {
    return mImageSemaphores[currentFrameIndex];
  };
  virtual ~SwapChain();

 private:
  VkSwapchainKHR mSwapChainHandle;
  DeviceRef mDevice;
  std::vector<VkImage> mSwapChainImages;
  std::vector<VkSemaphore> mImageSemaphores;

  const VkPresentModeKHR getPresentMode(
      const std::vector<VkPresentModeKHR> &presentModes) const;
  SwapChain(const SwapChain &) = delete;
};

using SwapChainRef = std::shared_ptr<SwapChain>;

}  // namespace Vulkan
}  // namespace Rendering
