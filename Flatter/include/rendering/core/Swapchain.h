#pragma once

#include <vulkan/vulkan.hpp>

#include "rendering//core/Surface.h"

namespace Rendering {
namespace Vulkan {
class FrameController;
using FrameControllerRef = std::shared_ptr<FrameController>;

const unsigned int PRESENT_MODE_MAILBOX_IMAGE_COUNT = 3;
const unsigned int PRESENT_MODE_DEFAULT_IMAGE_COUNT = 2;

class Swapchain {
 public:
  Swapchain(const vk::Device& device,
            const vk::PhysicalDevice& physicalDevice,
            const SurfaceRef& surface);
  const unsigned int acquireNextImage(const vk::Fence& waitFrameReady,
                                      const vk::Semaphore& signalImageReady);
  const vk::SwapchainKHR& getHandle() const { return mSwapchainHandle; };
  const vk::Extent2D& getExtent() const { return mSwapchainExtent; };
  const unsigned int getImageCount() const { return mImageCount; };
  const vk::Image& getImage(const unsigned int index) const {
    return mSwapchainImages[index];
  };
  virtual ~Swapchain();

 private:
  vk::SwapchainKHR mSwapchainHandle;

  unsigned int mImageCount;
  std::vector<vk::Image> mSwapchainImages;

  vk::Extent2D mSwapchainExtent;

  const vk::Device mDevice;
};

using SwapchainRef = std::shared_ptr<Swapchain>;
}  // namespace Vulkan
}  // namespace Rendering
