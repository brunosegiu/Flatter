#include "rendering/vulkan/IndexedVertexBuffer.h"

using namespace Rendering::Vulkan;

IndexedVertexBuffer::IndexedVertexBuffer(const ContextRef& context)
    : mContext(context) {
  mBindingDescription.setBinding(0)
      .setStride(sizeof(glm::vec3))
      .setInputRate(vk::VertexInputRate::eVertex);

  mAttributeDescription.setBinding(0).setLocation(0).setFormat(
      vk::Format::eR32G32B32Sfloat);

  // Vertex buffer
  const size_t vertexBufferSize = getVertexBufferSize();

  vk::Buffer stagingVertexBuffer{};
  vk::DeviceMemory stagingVertexBufferMemory{};
  mContext->createBuffer(vertexBufferSize,
                         vk::BufferUsageFlagBits::eTransferSrc,
                         vk::MemoryPropertyFlagBits::eHostVisible |
                             vk::MemoryPropertyFlagBits::eHostCoherent,
                         stagingVertexBuffer, stagingVertexBufferMemory);

  const vk::Device& device = mContext->getDevice();

  auto data = device.mapMemory(stagingVertexBufferMemory, 0, vertexBufferSize,
                               vk::MemoryMapFlags{});
  memcpy(data.value, mVertices.data(), vertexBufferSize);
  device.unmapMemory(stagingVertexBufferMemory);

  mContext->createBuffer(vertexBufferSize,
                         vk::BufferUsageFlagBits::eVertexBuffer |
                             vk::BufferUsageFlagBits::eTransferDst,
                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                         mVertexBuffer, mVertexBufferMemory);
  mContext->copyBuffer(stagingVertexBuffer, mVertexBuffer, vertexBufferSize);
  device.destroyBuffer(stagingVertexBuffer);
  device.freeMemory(stagingVertexBufferMemory);

  // Index buffer
  const size_t indexBufferSize = getIndexBufferSize();
  vk::Buffer stagingIndexBuffer{};
  vk::DeviceMemory stagingIndexBufferMemory{};
  mContext->createBuffer(indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                         vk::MemoryPropertyFlagBits::eHostVisible |
                             vk::MemoryPropertyFlagBits::eHostCoherent,
                         stagingIndexBuffer, stagingIndexBufferMemory);

  auto indexData = device.mapMemory(stagingIndexBufferMemory, 0,
                                    indexBufferSize, vk::MemoryMapFlags{});
  memcpy(indexData.value, mIndices.data(), indexBufferSize);
  device.unmapMemory(stagingIndexBufferMemory);

  mContext->createBuffer(indexBufferSize,
                         vk::BufferUsageFlagBits::eIndexBuffer |
                             vk::BufferUsageFlagBits::eTransferDst,
                         vk::MemoryPropertyFlagBits::eDeviceLocal, mIndexBuffer,
                         mIndexBufferMemory);
  mContext->copyBuffer(stagingIndexBuffer, mIndexBuffer, indexBufferSize);
  device.destroyBuffer(stagingIndexBuffer);
  device.freeMemory(stagingIndexBufferMemory);
}

IndexedVertexBuffer::~IndexedVertexBuffer() {}
