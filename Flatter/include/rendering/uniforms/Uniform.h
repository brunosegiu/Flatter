#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/core/Buffer.h"
#include "rendering/core/Context.h"
#include "rendering/uniforms/DescriptorLayout.h"
#include "rendering/uniforms/DescriptorPool.h"

namespace Rendering {
namespace Vulkan {
template <typename ValueType>
class Uniform {
 public:
  Uniform(const ContextRef& context,
          const DescriptorLayoutRef& layout,
          const DescriptorPoolRef& descriptorPool,
          const unsigned int binding,
          ValueType value,
          const size_t size = sizeof(ValueType));

  const vk::DescriptorSet& getDescriptorHandle() const {
    return mDescriptorSet;
  };

  void update(const ValueType& newValue);

  virtual ~Uniform(){};

 private:
  vk::DescriptorSet mDescriptorSet;

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
                            ValueType value,
                            const size_t size)
    : mContext(context) {
  const vk::Device& device = mContext->getDevice();

  vk::DeviceSize bufferSize{size};

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
                              .setRange(bufferSize);

  auto const descriptorWrite =
      vk::WriteDescriptorSet{}
          .setDstSet(mDescriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
          .setDescriptorCount(1)
          .setPBufferInfo(&bufferInfo);

  device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
  update(value);
}

template <typename ValueType>
void Uniform<ValueType>::update(const ValueType& newValue) {
  // TODO: Figure out a better way to access ptr
  mBuffer->map(&newValue[0]);
}
