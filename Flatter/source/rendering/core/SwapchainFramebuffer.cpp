﻿#include "rendering//core/SwapchainFramebuffer.h"

#include <array>

using namespace Rendering::Vulkan;

SwapchainFramebuffer::SwapchainFramebuffer(
    const ContextRef& context,
    const vk::Image& swapchainImage,
    const vk::Format& format,
    const vk::Extent2D& extent,
    const RenderPassRef& renderPass,
    const DepthBufferAttachmentRef& depthBuffer)
    : mContext(context) {
  auto const subresourceRange =
      vk::ImageSubresourceRange{}
          .setAspectMask(vk::ImageAspectFlagBits::eColor)
          .setBaseMipLevel(0)
          .setLevelCount(1)
          .setBaseArrayLayer(0)
          .setLayerCount(1);
  auto const imageViewCreateInfo = vk::ImageViewCreateInfo{}
                                       .setImage(swapchainImage)
                                       .setViewType(vk::ImageViewType::e2D)
                                       .setFormat(format)
                                       .setSubresourceRange(subresourceRange);

  const vk::Device& device = mContext->getDevice();
  assert(device.createImageView(&imageViewCreateInfo, nullptr,
                                &mSwapchainImageView) == vk::Result::eSuccess);

  std::vector<vk::ImageView> attachments;
  if (depthBuffer) {
    attachments = {mSwapchainImageView, depthBuffer->getImageView()};
  } else {
    attachments = {mSwapchainImageView};
  }

  auto const framebufferCreateInfo =
      vk::FramebufferCreateInfo{}
          .setRenderPass(renderPass->getHandle())
          .setAttachmentCount(static_cast<unsigned int>(attachments.size()))
          .setPAttachments(attachments.data())
          .setWidth(extent.width)
          .setHeight(extent.height)
          .setLayers(1);

  assert(device.createFramebuffer(&framebufferCreateInfo, nullptr,
                                  &mFramebufferHandle) == vk::Result::eSuccess);
}

SwapchainFramebuffer ::~SwapchainFramebuffer() {
  const vk::Device& device = mContext->getDevice();
  device.destroyFramebuffer(mFramebufferHandle, nullptr);
  device.destroyImageView(mSwapchainImageView, nullptr);
}