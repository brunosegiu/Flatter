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
  VkSwapchainKHR mSwapchainHandle;
  unsigned int mSwapchainImageCount;
  std::vector<VkImage> mSwapchainImages;  // MAX_SWAPCHAIN_IMAGES
  VkExtent2D mSwapchainExtent;

  Swapchain(const Device& device, Surface& surface);

  virtual ~Swapchain();
};

}  // namespace Vulkan
}  // namespace Rendering
