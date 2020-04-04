﻿#pragma once

#include <memory>
#include <vector>

#include "rendering/vulkan/IndexedVertexBuffer.h"

namespace Rendering {

class Mesh {
 public:
  using MeshRef = std::shared_ptr<Mesh>;

  Mesh(Rendering::Vulkan::IndexedVertexBufferRef vertexBuffer);
  Mesh(const Rendering::Vulkan::ContextRef& context,
       const std::vector<glm::vec3>& vertices,
       const std::vector<unsigned int>& indices);

  const Rendering::Vulkan::IndexedVertexBufferRef& getIndexedVertexBuffer()
      const {
    return mVertexBuffer;
  };

  virtual ~Mesh();

 private:
  Mesh(Mesh const&) = delete;
  Mesh& operator=(Mesh const&) = delete;

  Rendering::Vulkan::IndexedVertexBufferRef mVertexBuffer;
  std::vector<MeshRef> mChildren;
};

using MeshRef = std::shared_ptr<Mesh>;

}  // namespace Rendering
