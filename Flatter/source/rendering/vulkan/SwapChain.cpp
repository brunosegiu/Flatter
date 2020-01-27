#include "rendering/vulkan/SwapChain.h"

#include <assert.h>
#include <algorithm>

using namespace Rendering::Vulkan;

SwapChain::SwapChain(const Rendering::Vulkan::Device &device,
                     const Rendering::Vulkan::Surface &surface,
                     const unsigned int width, const unsigned int height) {
  unsigned int formatCount = 1;
  VkSurfaceFormatKHR surfaceFormat;
  const VkPhysicalDevice physicalDevice(device.getPhysicalDeviceHandle());
  const VkSurfaceKHR surfaceHandle(surface.getSurfaceHandle());
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surfaceHandle,
                                       &formatCount, &surfaceFormat);
  surfaceFormat.format = surfaceFormat.format == VK_FORMAT_UNDEFINED
                             ? VK_FORMAT_B8G8R8A8_UNORM
                             : surfaceFormat.format;
  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surfaceHandle,
                                            &presentModeCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount,
                                             VK_PRESENT_MODE_MAX_ENUM_KHR);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surfaceHandle, &presentModeCount, presentModes.data());

  const VkPresentModeKHR presentMode(getPresentMode(presentModes));
  unsigned int swapchainImageCount = 2;

  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surfaceHandle,
                                            &surfaceCapabilities);
  VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;
  if (swapchainExtent.width == UINT32_MAX) {
    swapchainExtent.width = std::clamp<unsigned int>(
        width, surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width);
    swapchainExtent.height = std::clamp<unsigned int>(
        height, surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height);
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = surfaceHandle;
  swapChainCreateInfo.minImageCount = swapchainImageCount;
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent = swapchainExtent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.pNext = 0;

  const VkDevice deviceHandle(device.getDeviceHandle());

  VkResult result =
      vkCreateSwapchainKHR(deviceHandle, &swapChainCreateInfo, 0, &mSwapChain);

  assert(result == VK_SUCCESS);

  vkGetSwapchainImagesKHR(deviceHandle, mSwapChain, &swapchainImageCount, NULL);
  mSwapChainImages = std::vector<VkImage>(swapchainImageCount, nullptr);
  vkGetSwapchainImagesKHR(deviceHandle, mSwapChain, &swapchainImageCount,
                          mSwapChainImages.data());
}

const VkPresentModeKHR SwapChain::getPresentMode(
    const std::vector<VkPresentModeKHR> &presentModes) const {
  const auto it =
      std::find_if(presentModes.begin(), presentModes.end(),
                   [](const VkPresentModeKHR &presentMode) {
                     return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
                   });
  VkPresentModeKHR presentMode = it == presentModes.end()
                                     ? VK_PRESENT_MODE_FIFO_KHR
                                     : VK_PRESENT_MODE_MAILBOX_KHR;
  return presentMode;
}