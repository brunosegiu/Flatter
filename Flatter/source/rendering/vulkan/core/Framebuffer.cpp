#include "rendering/vulkan/core/Framebuffer.h"

using namespace Rendering::Vulkan;

Framebuffer::Framebuffer(const ContextRef& context,
                         const vk::Image& swapchainImage,
                         const vk::Format& format,
                         const vk::Extent2D& extent,
                         const RenderPassRef& renderPass)
    : mContext(context) {
  auto const subresourceRange =
      vk::ImageSubresourceRange()
          .setAspectMask(vk::ImageAspectFlagBits::eColor)
          .setBaseMipLevel(0)
          .setLevelCount(1)
          .setBaseArrayLayer(0)
          .setLayerCount(1);
  auto const imageViewCreateInfo = vk::ImageViewCreateInfo()
                                       .setImage(swapchainImage)
                                       .setViewType(vk::ImageViewType::e2D)
                                       .setFormat(format)
                                       .setSubresourceRange(subresourceRange);

  const vk::Device& device = mContext->getDevice();
  device.createImageView(&imageViewCreateInfo, nullptr,
                         &mSwapchainImageViewHandle);

  auto const framebufferCreateInfo =
      vk::FramebufferCreateInfo()
          .setRenderPass(renderPass->getHandle())
          .setAttachmentCount(1)
          .setPAttachments(&mSwapchainImageViewHandle)
          .setWidth(extent.width)
          .setHeight(extent.height)
          .setLayers(1);

  device.createFramebuffer(&framebufferCreateInfo, nullptr,
                           &mFramebufferHandle);
}

Framebuffer ::~Framebuffer() {
  const vk::Device& device = mContext->getDevice();
  device.destroyFramebuffer(mFramebufferHandle, nullptr);
  device.destroyImageView(mSwapchainImageViewHandle, nullptr);
}
