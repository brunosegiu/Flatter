#include "rendering/vulkan/Framebuffer.h"

using namespace Rendering::Vulkan;

Framebuffer::Framebuffer(const VkImage& swapchainImage,
                         const Device& device,
                         Surface& surface,
                         const Swapchain& swapchain,
                         const RenderPass& renderPass) {
  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = swapchainImage;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = surface.getSurfaceFormat(device).format;
  VkImageSubresourceRange subresourceRange{};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;
  createInfo.subresourceRange = subresourceRange;
  vkCreateImageView(device.mDeviceHandle, &createInfo, 0,
                    &mSwapchainImageViewHandle);

  VkFramebufferCreateInfo framebufferCreateInfo{};
  framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferCreateInfo.renderPass = renderPass.mRenderPassHandle;
  framebufferCreateInfo.attachmentCount = 1;
  framebufferCreateInfo.pAttachments = &mSwapchainImageViewHandle;
  framebufferCreateInfo.width = swapchain.mSwapchainExtent.width;
  framebufferCreateInfo.height = swapchain.mSwapchainExtent.height;
  framebufferCreateInfo.layers = 1;

  vkCreateFramebuffer(device.mDeviceHandle, &framebufferCreateInfo, 0,
                      &mFramebufferHandle);
}

Framebuffer ::~Framebuffer() {
  // vkDestroyFramebuffer(device, framebuffers[i], NULL);
  // vkDestroyImageView(device, swapchainImageViews[i], NULL);
}
