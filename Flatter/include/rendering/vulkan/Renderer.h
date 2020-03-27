#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/Camera.h"
#include "rendering/vulkan/ScreenFramebufferRing.h"
#include "rendering/vulkan/VertexBuffer.h"
#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Pipeline.h"
#include "rendering/vulkan/core/RenderPass.h"

namespace Rendering {
namespace Vulkan {

class Renderer {
 public:
  Renderer(const ContextRef& context, const SurfaceRef& surface);

  void draw(const Rendering::Camera& camera);

  virtual ~Renderer();

 private:
  vk::DescriptorSetLayout mDescriptorSetLayout;
  RenderPassRef mRenderPass;
  PipelineRef mPipeline;
  ScreenFramebufferRingRef mScreenFramebufferRing;

  VertexBuffer mVertices;

  const ContextRef mContext;

  // Helpers

  void beginCommand(const vk::CommandBuffer& commandBuffer);
  void setViewportConstrains(const vk::CommandBuffer& commandBuffer,
                             const vk::Extent2D extent);
  void beginRenderPass(const vk::CommandBuffer& commandBuffer,
                       const FramebufferRef& framebuffer,
                       const RenderPassRef& renderPass,
                       const vk::Extent2D extent,
                       const vk::Offset2D offset = vk::Offset2D(0, 0),
                       const vk::ClearValue clearValue = vk::ClearColorValue(
                           std::array<float, 4>({{0, 0, 0, 0}})));
  void bindPipeline(const vk::CommandBuffer& commandBuffer,
                    const PipelineRef& pipeline);
  void bindUniforms(const vk::CommandBuffer& commandBuffer,
                    const UniformMatrixRef& uniformMatrix,
                    const PipelineRef& pipeline);
  void draw(const vk::CommandBuffer& commandBuffer);
  void endCommand(const vk::CommandBuffer& commandBuffer);
  void present(const vk::CommandBuffer& commandBuffer,
               const vk::Semaphore& imageAvailableSemaphore,
               const vk::Semaphore& renderingDoneSemaphore,
               const vk::Fence& presentFrameFence,
               const unsigned int imageIndex);
};

using RendererRef = std::shared_ptr<Renderer>;

}  // namespace Vulkan
}  // namespace Rendering
