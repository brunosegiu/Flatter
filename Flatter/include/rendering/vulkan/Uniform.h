#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

template <typename ValueType>
class Uniform {
 public:
  Uniform(const SingleDeviceRef& device,
          const vk::DescriptorSetLayout& descriptorSetLayout,
          ValueType value);

  const vk::DescriptorSet& getDescriptorHandle() const {
    return mDescriptorSet;
  };
  void update(const ValueType& newValue);

  virtual ~Uniform(){};

 private:
  vk::DescriptorSet mDescriptorSet;
  ValueType mValue;

  vk::Buffer mUniformBuffer;
  vk::DeviceMemory mUniformBufferMemory;

  SingleDeviceRef mDevice;
};

using UniformMatrixRef = std::shared_ptr<Uniform<glm::mat4>>;

}  // namespace Vulkan
}  // namespace Rendering

using namespace Rendering::Vulkan;

template <typename ValueType>
Uniform<ValueType>::Uniform(const SingleDeviceRef& device,
                            const vk::DescriptorSetLayout& descriptorSetLayout,
                            ValueType value)
    : mValue(value), mDevice(device) {
  vk::DeviceSize bufferSize = sizeof(ValueType);

  mDevice->allocBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                       vk::MemoryPropertyFlagBits::eHostVisible |
                           vk::MemoryPropertyFlagBits::eHostCoherent,
                       mUniformBuffer, mUniformBufferMemory);

  const vk::DescriptorPool& descriptorPool = mDevice->getDescriptorPool();
  auto const allocInfo = vk::DescriptorSetAllocateInfo()
                             .setDescriptorPool(descriptorPool)
                             .setDescriptorSetCount(1)
                             .setPSetLayouts(&descriptorSetLayout);
  mDevice->allocateDescriptorSets(&allocInfo, &mDescriptorSet);

  auto const bufferInfo = vk::DescriptorBufferInfo()
                              .setBuffer(mUniformBuffer)
                              .setOffset(0)
                              .setRange(sizeof(ValueType));

  auto const descriptorWrite =
      vk::WriteDescriptorSet()
          .setDstSet(mDescriptorSet)
          .setDstBinding(0)
          .setDstArrayElement(0)
          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
          .setDescriptorCount(1)
          .setPBufferInfo(&bufferInfo);

  mDevice->updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

template <typename ValueType>
void Uniform<ValueType>::update(const ValueType& newValue) {
  if (newValue != mValue) {
    mValue = newValue;
    auto data = mDevice->mapMemory(mUniformBufferMemory, 0, sizeof(ValueType),
                                   vk::MemoryMapFlags());
    memcpy(data.value, &mValue, sizeof(ValueType));
    mDevice->unmapMemory(mUniformBufferMemory);
  }
}
