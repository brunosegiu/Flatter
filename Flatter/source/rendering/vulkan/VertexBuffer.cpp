#include "rendering/vulkan/VertexBuffer.h"

using namespace Rendering::Vulkan;

VertexBuffer::VertexBuffer(const ContextRef& context) : mContext(context) {
  mDescription = vk::VertexInputBindingDescription{}
                     .setBinding(0)
                     .setStride(sizeof(glm::vec3))
                     .setInputRate(vk::VertexInputRate::eVertex);

  mAttrDescription = vk::VertexInputAttributeDescription{}
                         .setBinding(0)
                         .setLocation(0)
                         .setFormat(vk::Format::eR32G32B32Sfloat)
                         .setOffset(0);

  mContext->allocBuffer(mVertices.size() * sizeof(float),
                        vk::BufferUsageFlagBits::eVertexBuffer,
                        vk::MemoryPropertyFlagBits::eHostVisible |
                            vk::MemoryPropertyFlagBits::eHostCoherent,
                        mBuffer, mMemory);
  update();
}

void VertexBuffer::update() {
  const vk::Device& device = mContext->getDevice();
  auto data = device.mapMemory(mMemory, 0, mVertices.size() * sizeof(float),
                               vk::MemoryMapFlags());
  memcpy(data.value, mVertices.data(), mVertices.size() * sizeof(float));
  device.unmapMemory(mMemory);
}

VertexBuffer::~VertexBuffer() {}
