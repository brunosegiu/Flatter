#include "rendering//IndexedVertexBuffer.h"

using namespace Rendering::Vulkan;

vk::VertexInputBindingDescription IndexedVertexBuffer::sBindingDescription =
    vk::VertexInputBindingDescription{}
        .setBinding(0)
        .setStride(sizeof(glm::vec3))
        .setInputRate(vk::VertexInputRate::eVertex);

vk::VertexInputAttributeDescription IndexedVertexBuffer::sAttributeDescription =
    vk::VertexInputAttributeDescription{}
        .setBinding(0)
        .setLocation(0)
        .setFormat(vk::Format::eR32G32B32Sfloat);

vk::IndexType IndexedVertexBuffer::sIndexType = vk::IndexType::eUint16;

IndexedVertexBuffer::IndexedVertexBuffer(const ContextRef& context,
                                         const std::vector<glm::vec3>& vertices,
                                         const std::vector<uint16_t>& indices)
    : mContext(context),
      mVertexCount(static_cast<unsigned int>(vertices.size())),
      mIndexCount(static_cast<unsigned int>(indices.size())) {
  // Vertex buffer
  const size_t vertexBufferSize = getVertexBufferSize();

  Buffer stagingVertexBuffer(
      mContext, vertexBufferSize,
      vk::BufferUsageFlags(vk::BufferUsageFlagBits::eTransferSrc),
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent);
  const vk::Device& device = mContext->getDevice();
  stagingVertexBuffer.map(vertices.data());
  mVertexBuffer = std::make_shared<Buffer>(
      mContext, vertexBufferSize,
      vk::BufferUsageFlags(vk::BufferUsageFlagBits::eVertexBuffer |
                           vk::BufferUsageFlagBits::eTransferDst),
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  stagingVertexBuffer.copy(mVertexBuffer);

  // Index buffer
  const size_t indexBufferSize = getIndexBufferSize();
  Buffer stagingIndexBuffer(
      mContext, indexBufferSize,
      vk::BufferUsageFlags(vk::BufferUsageFlagBits::eTransferSrc),
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent);
  stagingIndexBuffer.map(indices.data());
  mIndexBuffer = std::make_shared<Buffer>(
      mContext, indexBufferSize,
      vk::BufferUsageFlags(vk::BufferUsageFlagBits::eIndexBuffer |
                           vk::BufferUsageFlagBits::eTransferDst),
      vk::MemoryPropertyFlagBits::eDeviceLocal);
  stagingIndexBuffer.copy(mIndexBuffer);
}

void IndexedVertexBuffer::draw(const vk::CommandBuffer& commandBuffer) {
  vk::DeviceSize offset{0};
  commandBuffer.bindVertexBuffers(0, 1, &getVertexBuffer(), &offset);
  commandBuffer.bindIndexBuffer(getIndexBuffer(), offset,
                                IndexedVertexBuffer::sIndexType);
  commandBuffer.drawIndexed(getIndexCount(), 1, 0, 0, 0);
}

IndexedVertexBuffer::~IndexedVertexBuffer() {}