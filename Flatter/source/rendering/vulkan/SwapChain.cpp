#include "rendering/vulkan/Swapchain.h"

#include <assert.h>

#include <algorithm>

using namespace Rendering::Vulkan;

Swapchain::Swapchain(const DeviceRef& device, const SurfaceRef& surface)
    : mDevice(device) {
  unsigned int presentModeCount = 0;
  const VkPhysicalDevice& physicalDeviceHandle = device->getPhysicalHandle();
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle,
                                            surface->mSurfaceHandle,
                                            &presentModeCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount,
                                             VK_PRESENT_MODE_FIFO_KHR);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDeviceHandle, surface->mSurfaceHandle, &presentModeCount,
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
      surface->getCapabilities(device);

  mSwapchainExtent = surfaceCapabilities.currentExtent;
  if (mSwapchainExtent.width == UINT32_MAX) {
    mSwapchainExtent.width = std::clamp<unsigned int>(
        surface->mWidth, surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width);
    mSwapchainExtent.height = std::clamp<unsigned int>(
        surface->mHeight, surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height);
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};

  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = surface->mSurfaceHandle;
  swapChainCreateInfo.minImageCount = mSwapchainImageCount;
  const VkSurfaceFormatKHR surfaceFormat = surface->getSurfaceFormat(device);
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
  const VkDevice& deviceHandle = device->getHandle();
  VkResult result = vkCreateSwapchainKHR(deviceHandle, &swapChainCreateInfo, 0,
                                         &mSwapchainHandle);
  assert(result == VK_SUCCESS);
  vkGetSwapchainImagesKHR(deviceHandle, mSwapchainHandle, &mSwapchainImageCount,
                          NULL);
  mSwapchainImages = std::vector<VkImage>(mSwapchainImageCount, nullptr);
  vkGetSwapchainImagesKHR(deviceHandle, mSwapchainHandle, &mSwapchainImageCount,
                          mSwapchainImages.data());
}

const unsigned int Swapchain::acquireNextImage(
    const VkFence& waitFor,
    const VkSemaphore& signalSemaphore) const {
  const VkDevice& deviceHandle = mDevice->getHandle();
  vkWaitForFences(deviceHandle, 1, &waitFor, VK_TRUE, UINT64_MAX);
  vkResetFences(deviceHandle, 1, &waitFor);
  unsigned int imageIndex = 0;
  vkAcquireNextImageKHR(deviceHandle, mSwapchainHandle, UINT64_MAX,
                        signalSemaphore, VK_NULL_HANDLE, &imageIndex);
  return imageIndex;
}

Swapchain ::~Swapchain() {
  vkDestroySwapchainKHR(mDevice->getHandle(), mSwapchainHandle, 0);
}
