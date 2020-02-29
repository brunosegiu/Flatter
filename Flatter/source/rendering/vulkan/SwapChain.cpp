#include "rendering/vulkan/Swapchain.h"

#include <assert.h>

#include <algorithm>

using namespace Rendering::Vulkan;

Swapchain::Swapchain(const Device& device, Surface& surface) {
  unsigned int presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device.mPhysicalDeviceHandle,
                                            surface.mSurfaceHandle,
                                            &presentModeCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount,
                                             VK_PRESENT_MODE_FIFO_KHR);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      device.mPhysicalDeviceHandle, surface.mSurfaceHandle, &presentModeCount,
      presentModes.data());

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
  for (uint32_t i = 0; i < presentModeCount; ++i) {
    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }
  }
  mSwapchainImageCount = presentMode == VK_PRESENT_MODE_MAILBOX_KHR
                             ? PRESENT_MODE_MAILBOX_IMAGE_COUNT
                             : PRESENT_MODE_DEFAULT_IMAGE_COUNT;

  const VkSurfaceCapabilitiesKHR surfaceCapabilities =
      surface.getCapabilities(device);

  mSwapchainExtent = surfaceCapabilities.currentExtent;
  if (mSwapchainExtent.width == UINT32_MAX) {
    mSwapchainExtent.width = std::clamp<unsigned int>(
        surface.mWidth, surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width);
    mSwapchainExtent.height = std::clamp<unsigned int>(
        surface.mHeight, surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height);
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};

  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = surface.mSurfaceHandle;
  swapChainCreateInfo.minImageCount = mSwapchainImageCount;
  const VkSurfaceFormatKHR surfaceFormat = surface.getSurfaceFormat(device);
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent = mSwapchainExtent;
  swapChainCreateInfo.imageArrayLayers = 1;  // 2 for stereo
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  VkResult result = vkCreateSwapchainKHR(
      device.mDeviceHandle, &swapChainCreateInfo, 0, &mSwapchainHandle);
  assert(result == VK_SUCCESS);
  vkGetSwapchainImagesKHR(device.mDeviceHandle, mSwapchainHandle,
                          &mSwapchainImageCount, NULL);
  mSwapchainImages = std::vector<VkImage>(mSwapchainImageCount, nullptr);
  vkGetSwapchainImagesKHR(device.mDeviceHandle, mSwapchainHandle,
                          &mSwapchainImageCount, mSwapchainImages.data());
}

Swapchain ::~Swapchain() {
  //  vkDestroySwapchainKHR(device, mSwapchainHandle, 0);
}
