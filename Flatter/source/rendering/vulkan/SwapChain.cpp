#include "rendering/vulkan/SwapChain.h"

#include <assert.h>

#include <algorithm>

using namespace Rendering::Vulkan;

SwapChain::SwapChain(const Rendering::Vulkan::DeviceRef &device,
                     const Rendering::Vulkan::Surface &surface,
                     unsigned int frameCount, const unsigned int width,
                     const unsigned int height)
    : mDevice(device),
      mSwapChainImages(frameCount, nullptr),
      mImageSemaphores(frameCount, nullptr) {
  unsigned int formatCount = 1;
  VkSurfaceFormatKHR surfaceFormat;
  const VkPhysicalDevice physicalDevice(device->getPhysicalDeviceHandle());
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
  swapChainCreateInfo.minImageCount = frameCount;
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
  swapChainCreateInfo.pNext = nullptr;

  const VkDevice deviceHandle(device->getNativeHandle());

  VkResult result = vkCreateSwapchainKHR(deviceHandle, &swapChainCreateInfo, 0,
                                         &mSwapChainHandle);

  assert(result == VK_SUCCESS);

  vkGetSwapchainImagesKHR(deviceHandle, mSwapChainHandle, &frameCount, NULL);
  vkGetSwapchainImagesKHR(deviceHandle, mSwapChainHandle, &frameCount,
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

void SwapChain::acquireImage(const unsigned int &currentFrameIndex) {
  unsigned int imageIndex;
  vkAcquireNextImageKHR(mDevice->getNativeHandle(), mSwapChainHandle,
                        UINT64_MAX, mImageSemaphores[currentFrameIndex],
                        VK_NULL_HANDLE, &imageIndex);
}

SwapChain::~SwapChain() {
  vkDestroySwapchainKHR(mDevice->getNativeHandle(), mSwapChainHandle, 0);
}
