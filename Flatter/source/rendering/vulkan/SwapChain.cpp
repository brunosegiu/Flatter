#include "rendering/vulkan/SwapChain.h"

#include <assert.h>

using namespace Rendering::Vulkan;
/*
SwapChain::SwapChain() {
  VkSurfaceFormatKHR surfaceFormat;
  surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
  surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

  VkExtent2D extent = {1280, 720};

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
}*/