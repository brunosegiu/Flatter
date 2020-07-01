#include "rendering//core/Buffer.h"

using namespace Rendering::Vulkan;

Buffer::Buffer(const ContextRef& context,
               const size_t bufferSize,
               const vk::BufferUsageFlags usage,
               const vk::MemoryPropertyFlags memoryProperty)
    : mContext(context), mBufferSize(bufferSize) {
  const vk::Device& device = mContext->getDevice();
  auto const bufferCreateInfo =
      vk::BufferCreateInfo{}
          .setSize(mBufferSize)
          .setUsage(usage)
          .setSharingMode(vk::SharingMode::eExclusive);
  device.createBuffer(&bufferCreateInfo, nullptr, &mBuffer);

  vk::MemoryRequirements memRequirements{};
  device.getBufferMemoryRequirements(mBuffer, &memRequirements);

  auto const allocCreateInfo =
      vk::MemoryAllocateInfo{}
          .setAllocationSize(memRequirements.size)
          .setMemoryTypeIndex(mContext->findBufferMemoryType(
              memRequirements.memoryTypeBits, memoryProperty));

  assert(device.allocateMemory(&allocCreateInfo, nullptr, &mBufferMemory) ==
         vk::Result::eSuccess);

  assert(device.bindBufferMemory(mBuffer, mBufferMemory, 0) ==
         vk::Result::eSuccess);
}

void Buffer::copy(const BufferRef& target) {
  const auto commandAllocInfo = vk::CommandBufferAllocateInfo{}
                                    .setLevel(vk::CommandBufferLevel::ePrimary)
                                    .setCommandPool(mContext->getCommandPool())
                                    .setCommandBufferCount(1);
  vk::CommandBuffer commandBuffer{};
  mContext->getDevice().allocateCommandBuffers(&commandAllocInfo,
                                               &commandBuffer);

  const auto commandBeginInfo = vk::CommandBufferBeginInfo{}.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  commandBuffer.begin(&commandBeginInfo);
  const auto region =
      vk::BufferCopy{}.setSrcOffset(0).setDstOffset(0).setSize(mBufferSize);
  commandBuffer.copyBuffer(mBuffer, target->getBuffer(), region);
  commandBuffer.end();

  const auto commandSubmitInfo =
      vk::SubmitInfo{}.setCommandBufferCount(1).setPCommandBuffers(
          &commandBuffer);
  mContext->getQueue().submit(commandSubmitInfo, vk::Fence());
  mContext->getQueue().waitIdle();
  mContext->getDevice().freeCommandBuffers(mContext->getCommandPool(),
                                           commandBuffer);
}

void Buffer::map(const void* hostData) {
  const vk::Device device = mContext->getDevice();
  auto data =
      device.mapMemory(mBufferMemory, 0, mBufferSize, vk::MemoryMapFlags{});
  assert(data.result == vk::Result::eSuccess);
  memcpy(data.value, hostData, mBufferSize);
  device.unmapMemory(mBufferMemory);
}

Buffer::~Buffer() {
  const vk::Device& device = mContext->getDevice();
  device.destroyBuffer(mBuffer);
  device.freeMemory(mBufferMemory);
}