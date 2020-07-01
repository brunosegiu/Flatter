#include "Entity.h"

Entity::Entity(const TransformRef& transform,
               const Rendering::Vulkan::IndexedVertexBufferRef& geometry)
    : mTransform(transform), mGeometry(geometry) {
  if (!mTransform) {
    mTransform = std::make_shared<Transform>();
  }
}

void Entity::draw(const Rendering::Camera& camera,
                  const Rendering::Vulkan::PipelineRef& pipeline,
                  const vk::CommandBuffer& commandBuffer) {
  commandBuffer.pushConstants(pipeline->getPipelineLayout(),
                              vk::ShaderStageFlagBits::eVertex, 0,
                              sizeof(glm::mat4), &getTransform()->getMatrix());
  mGeometry->draw(commandBuffer);
}