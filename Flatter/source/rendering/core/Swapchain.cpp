#include "rendering//core/Swapchain.h"

#include <assert.h>

#include <algorithm>

using namespace Rendering::Vulkan;

Swapchain::Swapchain(const vk::Device& device,
                     const vk::PhysicalDevice& physicalDevice,
                     const SurfaceRef& surface)
    : mDevice(device) {
  unsigned int presentModeCount = 0;

  physicalDevice.getSurfacePresentModesKHR(
      surface->mSurfaceHandle, &presentModeCount,
      static_cast<vk::PresentModeKHR*>(nullptr));
  std::vector<vk::PresentModeKHR> presentModes(presentModeCount,
                                               vk::PresentModeKHR::eFifo);
  physicalDevice.getSurfacePresentModesKHR(
      surface->mSurfaceHandle, &presentModeCount, presentModes.data());

  vk::PresentModeKHR presentMode =
      std::find(presentModes.begin(), presentModes.end(),
                vk::PresentModeKHR::eFifo) != presentModes.end()
          ? vk::PresentModeKHR::eFifo
          : vk::PresentModeKHR::eMailbox;

  mImageCount = presentMode == vk::PresentModeKHR::eFifo
                    ? PRESENT_MODE_DEFAULT_IMAGE_COUNT
                    : PRESENT_MODE_MAILBOX_IMAGE_COUNT;

  const vk::SurfaceCapabilitiesKHR surfaceCapabilities =
      surface->getCapabilities(physicalDevice);

  mSwapchainExtent = surfaceCapabilities.currentExtent;
  if (mSwapchainExtent.width == UINT32_MAX) {
    mSwapchainExtent.width = std::clamp<unsigned int>(
        surface->mWidth, surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width);
    mSwapchainExtent.height = std::clamp<unsigned int>(
        surface->mHeight, surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height);
  }

  const vk::SurfaceFormatKHR& surfaceFormat =
      surface->getFormat(physicalDevice);
  const auto swapChainCreateInfo =
      vk::SwapchainCreateInfoKHR()
          .setSurface(surface->mSurfaceHandle)
          .setMinImageCount(mImageCount)
          .setImageFormat(surfaceFormat.format)
          .setImageColorSpace(surfaceFormat.colorSpace)
          .setImageExtent(mSwapchainExtent)
          .setImageArrayLayers(1)
          .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
          .setImageSharingMode(vk::SharingMode::eExclusive)
          .setPreTransform(surfaceCapabilities.currentTransform)
          .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
          .setPresentMode(presentMode)
          .setClipped(VK_TRUE);

  assert(mDevice.createSwapchainKHR(&swapChainCreateInfo, nullptr,
                                    &mSwapchainHandle) == vk::Result::eSuccess);
  mDevice.getSwapchainImagesKHR(mSwapchainHandle, &mImageCount,
                                static_cast<vk::Image*>(nullptr));
  mSwapchainImages = std::vector<vk::Image>(mImageCount, vk::Image{});
  mDevice.getSwapchainImagesKHR(mSwapchainHandle, &mImageCount,
                                mSwapchainImages.data());
}

const unsigned int Swapchain::acquireNextImage(
    const vk::Fence& waitFrameReady,
    const vk::Semaphore& signalImageReady) {
  const vk::Result waitResult =
      mDevice.waitForFences(1, &waitFrameReady, VK_TRUE, UINT64_MAX);
  assert(waitResult == vk::Result::eSuccess);
  assert(mDevice.resetFences(1, &waitFrameReady) == vk::Result::eSuccess);

  unsigned int imageIndex = 0;
  assert(mDevice.acquireNextImageKHR(mSwapchainHandle, UINT64_MAX,
                                     signalImageReady, vk::Fence{},
                                     &imageIndex) == vk::Result::eSuccess);
  return imageIndex;
}

Swapchain::~Swapchain() {
  mDevice.destroySwapchainKHR(mSwapchainHandle, nullptr);
}