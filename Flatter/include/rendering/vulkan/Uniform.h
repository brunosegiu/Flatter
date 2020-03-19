#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

template <typename ValueType>
class Uniform {
 public:
  Uniform(const DeviceRef& device,
          const VkDescriptorSetLayout& descriptorSetLayout,
          ValueType value);

  const VkDescriptorSet& getDescriptorHandle() const { return mDescriptorSet; };
  void update(const ValueType& newValue);

  virtual ~Uniform(){};

 private:
  VkDescriptorSet mDescriptorSet;
  ValueType mValue;

  VkBuffer mUniformBuffer;
  VkDeviceMemory mUniformBufferMemory;

  DeviceRef mDevice;
};

using UniformMatrixRef = std::shared_ptr<Uniform<glm::mat4>>;

}  // namespace Vulkan
}  // namespace Rendering

using namespace Rendering::Vulkan;

template <typename ValueType>
Uniform<ValueType>::Uniform(const DeviceRef& device,
                            const VkDescriptorSetLayout& descriptorSetLayout,
                            ValueType value)
    : mValue(value), mDevice(device) {
  VkDeviceSize bufferSize = sizeof(ValueType);

  mDevice->allocBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       mUniformBuffer, mUniformBufferMemory);

  const VkDescriptorPool& descriptorPool = mDevice->getDescriptorPool();
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &descriptorSetLayout;

  vkAllocateDescriptorSets(device->getHandle(), &allocInfo, &mDescriptorSet);

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = mUniformBuffer;
  bufferInfo.offset = 0;
  bufferInfo.range = sizeof(ValueType);

  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = mDescriptorSet;
  descriptorWrite.dstBinding = 0;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pBufferInfo = &bufferInfo;

  vkUpdateDescriptorSets(device->getHandle(), 1, &descriptorWrite, 0, nullptr);
}

template <typename ValueType>
void Uniform<ValueType>::update(const ValueType& newValue) {
  if (newValue != mValue) {
    mValue = newValue;
    void* data = nullptr;
    vkMapMemory(mDevice->getHandle(), mUniformBufferMemory, 0,
                sizeof(ValueType), 0, &data);
    memcpy(data, &mValue, sizeof(ValueType));
    vkUnmapMemory(mDevice->getHandle(), mUniformBufferMemory);
  }
}
