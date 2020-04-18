#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/Camera.h"
#include "rendering/Scene.h"
#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/pipelines/SinglePassPipeline.h"
#include "rendering/vulkan/renderPasses/SingleRenderPass.h"
#include "rendering/vulkan/renderers/Renderer.h"

namespace Rendering {
namespace Vulkan {

class SinglePassRenderer : public Renderer {
 public:
  SinglePassRenderer(const ContextRef& context, const SurfaceRef& surface);

  void draw(Rendering::Camera& camera, const SceneRef& scene) override;

  virtual ~SinglePassRenderer();

 private:
  SinglePassRenderer(SinglePassRenderer const&) = delete;
  SinglePassRenderer& operator=(SinglePassRenderer const&) = delete;

  vk::DescriptorSetLayout mDescriptorSetLayout;
  SingleRenderPassRef mRenderPass;
  SinglePassPipelineRef mPipeline;

  DepthBufferAttachmentRef mDepthBuffer;

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
                    const SinglePassPipelineRef& pipeline);
  void bindUniforms(const vk::CommandBuffer& commandBuffer,
                    const UniformMatrixRef& uniformMatrix,
                    const SinglePassPipelineRef& pipeline);
  void draw(const vk::CommandBuffer& commandBuffer);
  void endCommand(const vk::CommandBuffer& commandBuffer);
  void present(const vk::CommandBuffer& commandBuffer,
               const vk::Semaphore& imageAvailableSemaphore,
               const vk::Semaphore& renderingDoneSemaphore,
               const vk::Fence& presentFrameFence,
               const unsigned int imageIndex);
};

using SinglePassRendererRef = std::shared_ptr<SinglePassRenderer>;

}  // namespace Vulkan
}  // namespace Rendering
