#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/Camera.h"
#include "rendering/Scene.h"
#include "rendering/vulkan/ScreenFramebufferRing.h"
#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Framebuffer.h"
#include "rendering/vulkan/pipelines/DeferredPipeline.h"
#include "rendering/vulkan/pipelines/FullscreenPipeline.h"
#include "rendering/vulkan/renderPasses/FullscreenRenderPass.h"
#include "rendering/vulkan/renderPasses/GBufferRenderPass.h"
#include "rendering/vulkan/renderers/Renderer.h"
#include "rendering/vulkan/uniforms/DescriptorLayout.h"
#include "rendering/vulkan/uniforms/DescriptorPool.h"
#include "rendering/vulkan/uniforms/Sampler.h"
#include "rendering/vulkan/uniforms/Uniform.h"

namespace Rendering {
namespace Vulkan {
class DeferredRenderer : public Renderer {
 public:
  DeferredRenderer(const ContextRef& context, const SurfaceRef& surface);

  void draw(Rendering::Camera& camera, const SceneRef& scene) override;

  virtual ~DeferredRenderer();

 private:
  DeferredRenderer(DeferredRenderer const&) = delete;
  DeferredRenderer& operator=(DeferredRenderer const&) = delete;

  DescriptorPoolRef mDescriptorPool;
  DescriptorLayoutRef mDeferredLayout;
  DescriptorLayoutRef mFullscreenLayout;

  DeferredPipelineRef mDeferredPipeline;
  UniformMatrixRef mMatrixUniform;
  vk::Semaphore mOffscreenSemaphore;
  FramebufferRef mGBuffer;

  FullscreenRenderPassRef mFullscreenRenderPass;
  FullscreenPipelineRef mFullscreenPipeline;
  SamplerRef mAlbedoSampler, mPositionSampler, mNormalSampler;

  FramebufferAttachmentRef mColorAtt, mPosAtt, mNormAtt;
  DepthBufferAttachmentRef mDepthAtt;

  vk::CommandBuffer mDeferredCB;

  vk::DescriptorSet mDescriptorSet;

  // Helpers
  void drawDeferred(const RenderingResources& resources,
                    Rendering::Camera& camera,
                    const SceneRef& scene);
  void drawFullscreen(const RenderingResources& resources);
  void present(const vk::CommandBuffer& commandBuffer,
               const vk::Semaphore& waitFor,
               const vk::Semaphore& signalReady,
               const vk::Fence& queueFence,
               unsigned int imageIndex);
  void submit(const vk::CommandBuffer& commandBuffer,
              const vk::Semaphore& waitFor,
              const vk::Semaphore& signalReady,
              const vk::Fence& queueFence = vk::Fence{});
};

using DeferredRendererRef = std::shared_ptr<DeferredRenderer>;
}  // namespace Vulkan
}  // namespace Rendering
