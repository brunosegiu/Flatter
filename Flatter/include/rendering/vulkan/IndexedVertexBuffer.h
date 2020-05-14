#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Buffer.h"
#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {
class IndexedVertexBuffer {
 public:
  IndexedVertexBuffer(const ContextRef& context,
                      const std::vector<glm::vec3>& vertices,
                      const std::vector<uint16_t>& indices);

  const size_t getVertexBufferSize() const {
    return mVertices.size() * sizeof(glm::vec3);
  };
  const size_t getIndexBufferSize() const {
    return mIndices.size() * sizeof(uint16_t);
  };
  const vk::Buffer& getVertexBuffer() const {
    return mVertexBuffer->getBuffer();
  };
  const vk::Buffer& getIndexBuffer() const {
    return mIndexBuffer->getBuffer();
  };
  const unsigned int getIndexCount() const { return mIndexCount; };

  static vk::VertexInputBindingDescription sBindingDescription;
  static vk::VertexInputAttributeDescription sAttributeDescription;

  virtual ~IndexedVertexBuffer();

 private:
  IndexedVertexBuffer(IndexedVertexBuffer const&) = delete;
  IndexedVertexBuffer& operator=(IndexedVertexBuffer const&) = delete;

  BufferRef mVertexBuffer;
  BufferRef mIndexBuffer;

  std::vector<glm::vec3> mVertices;
  std::vector<uint16_t> mIndices;
  const unsigned int mIndexCount;

  const ContextRef mContext;
};

using IndexedVertexBufferRef = std::shared_ptr<IndexedVertexBuffer>;
}  // namespace Vulkan
}  // namespace Rendering
