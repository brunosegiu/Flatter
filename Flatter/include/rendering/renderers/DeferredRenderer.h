﻿#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//ScreenFramebufferRing.h"
#include "rendering//core/Context.h"
#include "rendering//core/Framebuffer.h"
#include "rendering//pipelines/DeferredPipeline.h"
#include "rendering//pipelines/FullscreenPipeline.h"
#include "rendering//renderPasses/FullscreenRenderPass.h"
#include "rendering//renderPasses/GBufferRenderPass.h"
#include "rendering//renderers/Renderer.h"
#include "rendering//uniforms/DescriptorLayout.h"
#include "rendering//uniforms/DescriptorPool.h"
#include "rendering//uniforms/Sampler.h"
#include "rendering//uniforms/Uniform.h"
#include "rendering/Camera.h"
#include "rendering/Scene.h"
#include "rendering/lighting/LightController.h"

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

  LightControllerRef mLightController;

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
  vk::Fence mDeferredFence;

  vk::DescriptorSet mDescriptorSet;

  // Helpers
  void drawDeferred(const RenderingResources& resources,
                    Rendering::Camera& camera,
                    const SceneRef& scene);
  void drawFullscreen(const RenderingResources& resources,
                      const Rendering::Camera& camera);
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
