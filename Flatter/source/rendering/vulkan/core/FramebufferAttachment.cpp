#include "rendering/vulkan/core/FramebufferAttachment.h"

using namespace Rendering::Vulkan;

Rendering::Vulkan::FramebufferAttachment::FramebufferAttachment(
    const ContextRef& context,
    const vk::Extent2D& extent,
    const vk::Format& format,
    const vk::ImageUsageFlags& usage,
    const vk::ImageAspectFlags& viewAspect,
    const vk::ImageTiling tiling)
    : mContext(context) {
  const auto imageCreateInfo =
      vk::ImageCreateInfo{}
          .setImageType(vk::ImageType::e2D)
          .setFormat(format)
          .setExtent({extent.width, extent.height, 1})
          .setMipLevels(1)
          .setArrayLayers(1)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setTiling(tiling)
          .setUsage(usage)
          .setInitialLayout(vk::ImageLayout::eUndefined);
  const vk::Device& device = mContext->getDevice();
  const auto imageRes = device.createImage(imageCreateInfo, nullptr);
  assert(imageRes.result == vk::Result::eSuccess);
  mImage = imageRes.value;
  vk::MemoryRequirements memoryReqs{};
  device.getImageMemoryRequirements(mImage, &memoryReqs);

  const auto memAllocInfo =
      vk::MemoryAllocateInfo{}
          .setAllocationSize(memoryReqs.size)
          .setMemoryTypeIndex(mContext->findBufferMemoryType(
              memoryReqs.memoryTypeBits,
              vk::MemoryPropertyFlagBits::eDeviceLocal));
  device.allocateMemory(&memAllocInfo, nullptr, &mMemory);
  device.bindImageMemory(mImage, mMemory, 0);
  const auto subsurfaceRange = vk::ImageSubresourceRange{}
                                   .setAspectMask(viewAspect)
                                   .setBaseMipLevel(0)
                                   .setLevelCount(1)
                                   .setBaseArrayLayer(0)
                                   .setLayerCount(1);
  const auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
                                       .setViewType(vk::ImageViewType::e2D)
                                       .setFormat(format)
                                       .setSubresourceRange(subsurfaceRange)
                                       .setImage(mImage);
  const auto imageViewRes =
      device.createImageView(imageViewCreateInfo, nullptr);
  assert(imageViewRes.result == vk::Result::eSuccess);
  mView = imageViewRes.value;
}

Rendering::Vulkan::FramebufferAttachment::~FramebufferAttachment() {}
