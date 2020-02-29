#pragma once

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Surface.h"

namespace Rendering {
namespace Vulkan {

const unsigned int PRESENT_MODE_MAILBOX_IMAGE_COUNT = 3;
const unsigned int PRESENT_MODE_DEFAULT_IMAGE_COUNT = 2;

class Swapchain {
 public:
  Swapchain(const DeviceRef& device, const SurfaceRef& surface);
  const unsigned int acquireNextImage(const VkFence& waitFor,
                                      const VkSemaphore& signalSemaphore) const;
  const VkSwapchainKHR& getHandle() const { return mSwapchainHandle; };
  const VkExtent2D& getExtent() const { return mSwapchainExtent; };
  const unsigned int getImageCount() const { return mSwapchainImageCount; };
  const VkImage& getImage(const unsigned int index) const {
    return mSwapchainImages[index];
  };
  virtual ~Swapchain();

 private:
  VkSwapchainKHR mSwapchainHandle;
  unsigned int mSwapchainImageCount;
  std::vector<VkImage> mSwapchainImages;
  VkExtent2D mSwapchainExtent;
  const DeviceRef mDevice;
};

using SwapchainRef = std::shared_ptr<Swapchain>;

}  // namespace Vulkan
}  // namespace Rendering
