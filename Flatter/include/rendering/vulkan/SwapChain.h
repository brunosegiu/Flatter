#pragma once

#include <vulkan/vulkan.h>

#include <rendering/vulkan/Device.h>
#include <rendering/vulkan/Surface.h>

namespace Rendering {
namespace Vulkan {

class SwapChain {
 public:
  SwapChain(const Rendering::Vulkan::Device &device,
            const Rendering::Vulkan::Surface &surface, const unsigned int width,
            const unsigned int height);

  virtual ~SwapChain() = default;

 private:
  VkSwapchainKHR mSwapChain;
  std::vector<VkImage> mSwapChainImages;
  const VkPresentModeKHR getPresentMode(
      const std::vector<VkPresentModeKHR> &presentModes) const;
  SwapChain(const SwapChain &) = delete;
};

}  // namespace Vulkan
}  // namespace Rendering