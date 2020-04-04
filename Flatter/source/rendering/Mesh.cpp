#include "rendering/Mesh.h"

using namespace Rendering;

Mesh::Mesh(Rendering::Vulkan::IndexedVertexBufferRef vertexBuffer)
    : mVertexBuffer(vertexBuffer) {}

Mesh::Mesh(const Rendering::Vulkan::ContextRef& context,
           const std::vector<glm::vec3>& vertices,
           const std::vector<unsigned int>& indices) {
  mVertexBuffer = std::make_shared<Rendering::Vulkan::IndexedVertexBuffer>(
      context, vertices, indices);
}

Mesh::~Mesh() {}
