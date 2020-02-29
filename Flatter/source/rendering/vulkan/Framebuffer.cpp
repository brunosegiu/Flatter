#include "rendering/vulkan/Framebuffer.h"

using namespace Rendering::Vulkan;

Framebuffer::Framebuffer(const DeviceRef device, const VkImage& image,
                         const VkExtent2D& extent,
                         const VkFormat attachmentFormat)
    : mDevice(device) {
  VkImageViewCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
  imageCreateInfo.image = image;
  imageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageCreateInfo.format = attachmentFormat;

  VkImageSubresourceRange subresourceRange{};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;
  imageCreateInfo.subresourceRange = subresourceRange;
  const VkDevice& deviceHandle(mDevice->getNativeHandle());

  vkCreateImageView(deviceHandle, &imageCreateInfo, 0, &mImageViewHandle);

  VkFramebufferCreateInfo framebufferCreateInfo{};
  framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferCreateInfo.renderPass = renderPass;
  framebufferCreateInfo.attachmentCount = 1;
  framebufferCreateInfo.pAttachments = &mImageViewHandle;
  framebufferCreateInfo.width = extent.width;
  framebufferCreateInfo.height = extent.height;
  framebufferCreateInfo.layers = 1;
  vkCreateFramebuffer(deviceHandle, &framebufferCreateInfo, 0,
                      &mFramebufferHandle);
}

Framebuffer::~Framebuffer() {
  const VkDevice& deviceHandle(mDevice->getNativeHandle());
  vkDestroyFramebuffer(deviceHandle, mFramebufferHandle, NULL);
  vkDestroyImageView(deviceHandle, mImageViewHandle, NULL);
}
