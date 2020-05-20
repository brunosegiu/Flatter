#include "rendering/vulkan/uniforms/Sampler.h"

#include <assert.h>

using namespace Rendering::Vulkan;

Sampler::Sampler(const ContextRef& context,
                 const DescriptorPoolRef& descriptorPool,
                 const DescriptorLayoutRef& layout,
                 const FramebufferAttachmentRef& framebufferAttachment,
                 const unsigned int binding)
    : mContext(context) {
  // Create sampler to sample from the color attachments
  const auto samplerCreateInfo =
      vk::SamplerCreateInfo{}
          .setMagFilter(vk::Filter::eNearest)
          .setMinFilter(vk::Filter::eNearest)
          .setMipmapMode(vk::SamplerMipmapMode::eLinear)
          .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
          .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
          .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
          .setMipLodBias(0.0f)
          .setMaxAnisotropy(1.0f)
          .setMinLod(0.0f)
          .setMaxLod(1.0f)
          .setBorderColor(vk::BorderColor::eFloatOpaqueWhite);

  const vk::Device& device = mContext->getDevice();
  assert(device.createSampler(&samplerCreateInfo, nullptr, &mSampler) ==
         vk::Result::eSuccess);

  auto const descriptorAllocInfo =
      vk::DescriptorSetAllocateInfo{}
          .setDescriptorPool(descriptorPool->getDescriptorPool())
          .setDescriptorSetCount(1)
          .setPSetLayouts(&layout->getHandle());
  device.allocateDescriptorSets(&descriptorAllocInfo, &mDescriptorSet);

  const auto descriptorImageInfo =
      vk::DescriptorImageInfo{}
          .setSampler(mSampler)
          .setImageView(framebufferAttachment->getImageView())
          .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

  const auto writeDescriptorSet =
      vk::WriteDescriptorSet{}
          .setDstSet(mDescriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
          .setDescriptorCount(1)
          .setPImageInfo(&descriptorImageInfo);

  device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

Sampler::~Sampler() {
  mContext->getDevice().destroySampler(mSampler, nullptr);
}