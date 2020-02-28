#include "rendering/vulkan/Framebuffer.h"

using namespace Rendering::Vulkan;

Framebuffer::Framebuffer(const DeviceRef device, const SwapChainRef swapChain,
                         const VkFormat attachmentFormat)
    : mDevice(device) {
  VkImageViewCreateInfo icreateInfo{};
  icreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
  icreateInfo.image = swapchainImages[i];
  icreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  icreateInfo.format = attachmentFormat;

  VkImageSubresourceRange subresourceRange{};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;
  icreateInfo.subresourceRange = subresourceRange;
  const VkDevice& deviceHandle(mDevice->getNativeHandle());

  vkCreateImageView(deviceHandle, &icreateInfo, 0, &swapchainImageViews[i]);

  VkFramebufferCreateInfo framebufferCreateInfo{};
  framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferCreateInfo.renderPass = renderPass;
  framebufferCreateInfo.attachmentCount = 1;
  framebufferCreateInfo.pAttachments = &swapchainImageViews[i];
  framebufferCreateInfo.width = swapchainExtent.width;
  framebufferCreateInfo.height = swapchainExtent.height;
  framebufferCreateInfo.layers = 1;
  vkCreateFramebuffer(deviceHandle, &framebufferCreateInfo, 0,
                      &mFramebufferHandle);
}

Framebuffer::~Framebuffer() {
  const VkDevice& deviceHandle(mDevice->getNativeHandle());
  vkDestroyFramebuffer(deviceHandle, mFramebufferHandle, NULL);
  vkDestroyImageView(deviceHandle, swapchainImageViews[i], NULL);
}
