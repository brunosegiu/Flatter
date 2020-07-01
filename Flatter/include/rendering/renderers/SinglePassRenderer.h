#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//pipelines/SinglePassPipeline.h"
#include "rendering//renderPasses/SingleRenderPass.h"
#include "rendering//renderers/Renderer.h"
#include "rendering//uniforms/DescriptorLayout.h"
#include "rendering//uniforms/Uniform.h"
#include "rendering/Camera.h"
#include "rendering/Scene.h"

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

  DescriptorPoolRef mDescriptorPool;

  SingleRenderPassRef mRenderPass;
  SinglePassPipelineRef mPipeline;

  DescriptorLayoutRef mDescriptorLayout;
  UniformMatrixRef mMatrixUniform;

  DepthBufferAttachmentRef mDepthBuffer;

  // Helpers

  void beginCommand(const vk::CommandBuffer& commandBuffer);
  void setViewportConstrains(const vk::CommandBuffer& commandBuffer,
                             const vk::Extent2D extent);
  void beginRenderPass(const vk::CommandBuffer& commandBuffer,
                       const SwapchainFramebufferRef& framebuffer,
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
