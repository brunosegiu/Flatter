﻿#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Buffer.h"
#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/uniforms/DescriptorLayout.h"
#include "rendering/vulkan/uniforms/DescriptorPool.h"

namespace Rendering {
namespace Vulkan {
template <typename ValueType>
class Uniform {
 public:
  Uniform(const ContextRef& context,
          const DescriptorLayoutRef& layout,
          const DescriptorPoolRef& descriptorPool,
          const unsigned int binding,
          ValueType value);

  const vk::DescriptorSet& getDescriptorHandle() const {
    return mDescriptorSet;
  };

  void update(const ValueType& newValue);

  virtual ~Uniform(){};

 private:
  vk::DescriptorSet mDescriptorSet;
  ValueType mValue;

  BufferRef mBuffer;

  ContextRef mContext;
};

using UniformMatrixRef = std::shared_ptr<Uniform<glm::mat4>>;
}  // namespace Vulkan
}  // namespace Rendering

using namespace Rendering::Vulkan;

template <typename ValueType>
Uniform<ValueType>::Uniform(const ContextRef& context,
                            const DescriptorLayoutRef& layout,
                            const DescriptorPoolRef& descriptorPool,
                            const unsigned int binding,
                            ValueType value)
    : mValue(value), mContext(context) {
  const vk::Device& device = mContext->getDevice();

  vk::DeviceSize bufferSize = sizeof(ValueType);

  mBuffer = std::make_shared<Buffer>(
      mContext, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent);

  auto const allocInfo =
      vk::DescriptorSetAllocateInfo{}
          .setDescriptorPool(descriptorPool->getDescriptorPool())
          .setDescriptorSetCount(1)
          .setPSetLayouts(&layout->getHandle());
  device.allocateDescriptorSets(&allocInfo, &mDescriptorSet);

  auto const bufferInfo = vk::DescriptorBufferInfo{}
                              .setBuffer(mBuffer->getBuffer())
                              .setOffset(0)
                              .setRange(sizeof(ValueType));

  auto const descriptorWrite =
      vk::WriteDescriptorSet{}
          .setDstSet(mDescriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
          .setDescriptorCount(1)
          .setPBufferInfo(&bufferInfo);

  device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

template <typename ValueType>
void Uniform<ValueType>::update(const ValueType& newValue) {
  if (newValue != mValue) {
    mValue = newValue;
    mBuffer->map(&mValue);
  }
}
