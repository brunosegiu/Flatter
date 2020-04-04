#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {

class IndexedVertexBuffer {
 public:
  IndexedVertexBuffer(const ContextRef& context,
                      const std::vector<glm::vec3>& vertices,
                      const std::vector<unsigned int>& indices);

  const size_t getVertexBufferSize() const {
    return mVertices.size() * sizeof(glm::vec3);
  };

  const size_t getIndexBufferSize() const {
    return mIndices.size() * sizeof(unsigned int);
  };

  static vk::VertexInputBindingDescription sBindingDescription;
  static vk::VertexInputAttributeDescription sAttributeDescription;

  virtual ~IndexedVertexBuffer();

 public:
  IndexedVertexBuffer(IndexedVertexBuffer const&) = delete;
  IndexedVertexBuffer& operator=(IndexedVertexBuffer const&) = delete;

  vk::Buffer mVertexBuffer;
  vk::DeviceMemory mVertexBufferMemory;
  vk::Buffer mIndexBuffer;
  vk::DeviceMemory mIndexBufferMemory;

  std::vector<glm::vec3> mVertices;
  std::vector<unsigned int> mIndices;

  const ContextRef mContext;
};

using IndexedVertexBufferRef = std::shared_ptr<IndexedVertexBuffer>;
}  // namespace Vulkan
}  // namespace Rendering
