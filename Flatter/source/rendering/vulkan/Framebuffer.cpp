#include "rendering/vulkan/Framebuffer.h"

using namespace Rendering::Vulkan;

Framebuffer::Framebuffer(const SingleDeviceRef& device,
                         const vk::Image& swapchainImage,
                         const vk::Format& format,
                         const vk::Extent2D& extent,
                         const RenderPassRef& renderPass)
    : mDevice(device) {
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

  mDevice->createImageView(&imageViewCreateInfo, nullptr,
                           &mSwapchainImageViewHandle);

  auto const framebufferCreateInfo =
      vk::FramebufferCreateInfo()
          .setRenderPass(renderPass->getHandle())
          .setAttachmentCount(1)
          .setPAttachments(&mSwapchainImageViewHandle)
          .setWidth(extent.width)
          .setHeight(extent.height)
          .setLayers(1);

  mDevice->createFramebuffer(&framebufferCreateInfo, nullptr,
                             &mFramebufferHandle);
}

Framebuffer ::~Framebuffer() {
  mDevice->destroyFramebuffer(mFramebufferHandle, nullptr);
  mDevice->destroyImageView(mSwapchainImageViewHandle, nullptr);
}
