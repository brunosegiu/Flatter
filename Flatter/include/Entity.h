#pragma once

#include "Transform.h"
#include "rendering//IndexedVertexBuffer.h"
#include "rendering//pipelines/Pipeline.h"
#include "rendering/Camera.h"

class Entity {
  using EntityRef = std::shared_ptr<Entity>;

 public:
  Entity(const TransformRef& transform = nullptr,
         const Rendering::Vulkan::IndexedVertexBufferRef& geometry = nullptr);

  virtual void draw(const Rendering::Camera& camera,
                    const Rendering::Vulkan::PipelineRef& pipeline,
                    const vk::CommandBuffer& commandBuffer);

  const Rendering::Vulkan::IndexedVertexBufferRef& getGeometry() const {
    return mGeometry;
  };
  const TransformRef& getTransform() const { return mTransform; };

  void setTransform(const TransformRef& transform) { mTransform = transform; };
  void setGeometry(const Rendering::Vulkan::IndexedVertexBufferRef& geometry) {
    mGeometry = geometry;
  };
  void addChild(const EntityRef& child) { mChildren.push_back(child); };

  virtual ~Entity() = default;

 protected:
  TransformRef mTransform;
  Rendering::Vulkan::IndexedVertexBufferRef mGeometry;
  std::vector<EntityRef> mChildren;

 private:
  Entity(Entity const&) = delete;
  Entity& operator=(Entity const&) = delete;
};

using EntityRef = std::shared_ptr<Entity>;
