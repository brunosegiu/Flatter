#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"

const std::vector<glm::vec3> testVertices{
    {-1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f},
    {-1.0f, 1.0f, -1.0f}, {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
    {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}};

const std::vector<unsigned int> testIndices{1, 2, 0, 3, 6, 2, 7, 4, 6, 5, 0, 4,
                                            6, 1, 2, 3, 5, 7, 1, 3, 2, 3, 7, 6,
                                            7, 5, 4, 5, 1, 0, 6, 4, 0, 3, 1, 5};

namespace Rendering {
namespace Vulkan {

class IndexedVertexBuffer {
 public:
  IndexedVertexBuffer(const ContextRef& context);

  const size_t getVertexBufferSize() const {
    return mVertices.size() * sizeof(glm::vec3);
  };

  const size_t getIndexBufferSize() const {
    return mIndices.size() * sizeof(unsigned int);
  };

  virtual ~IndexedVertexBuffer();

 public:
  IndexedVertexBuffer(IndexedVertexBuffer const&) = delete;
  IndexedVertexBuffer& operator=(IndexedVertexBuffer const&) = delete;

  vk::VertexInputBindingDescription mBindingDescription;
  vk::VertexInputAttributeDescription mAttributeDescription;

  vk::Buffer mVertexBuffer;
  vk::DeviceMemory mVertexBufferMemory;
  vk::Buffer mIndexBuffer;
  vk::DeviceMemory mIndexBufferMemory;

  std::vector<glm::vec3> mVertices = testVertices;
  std::vector<unsigned int> mIndices = testIndices;

  const ContextRef mContext;
};

using IndexedVertexBufferRef = std::shared_ptr<IndexedVertexBuffer>;
}  // namespace Vulkan
}  // namespace Rendering
