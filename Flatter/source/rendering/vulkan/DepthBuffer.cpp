#include "rendering/vulkan/DepthBuffer.h"

using namespace Rendering::Vulkan;

DepthBuffer::DepthBuffer(const ContextRef& context, const vk::Extent2D& extent)
    : mContext(context) {
  const vk::Format format = mContext->findSupportedFormat(
      {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
       vk::Format::eD24UnormS8Uint},
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlags(
          vk::FormatFeatureFlagBits::eDepthStencilAttachment));
  const auto imageCreatInfo =
      vk::ImageCreateInfo{}
          .setImageType(vk::ImageType::e2D)
          .setFormat(format)
          .setExtent({extent.width, extent.height, 1})
          .setMipLevels(1)
          .setArrayLayers(1)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setTiling(vk::ImageTiling::eOptimal)
          .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
          .setInitialLayout(vk::ImageLayout::eUndefined);
  const vk::Device& device = mContext->getDevice();
  const auto imageRes = device.createImage(imageCreatInfo, nullptr);
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

  vk::ImageAspectFlags mask = (format == vk::Format::eD32Sfloat)
                                  ? vk::ImageAspectFlagBits::eDepth
                                  : (vk::ImageAspectFlagBits::eDepth |
                                     vk::ImageAspectFlagBits::eStencil);
  const auto subsurfaceRange = vk::ImageSubresourceRange{}
                                   .setAspectMask(mask)
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
  mImageView = imageViewRes.value;
}

DepthBuffer::~DepthBuffer() {}
