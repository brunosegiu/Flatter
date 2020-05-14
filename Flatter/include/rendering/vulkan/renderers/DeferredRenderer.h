#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/Camera.h"
#include "rendering/Scene.h"
#include "rendering/vulkan/ScreenFramebufferRing.h"
#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/pipelines/DeferredPipeline.h"
#include "rendering/vulkan/pipelines/FullscreenPipeline.h"
#include "rendering/vulkan/renderPasses/FullscreenRenderPass.h"
#include "rendering/vulkan/renderPasses/GBufferRenderPass.h"
#include "rendering/vulkan/renderers/Renderer.h"

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

  GBufferRenderPassRef mOffscreenRenderPass;
  DeferredPipeline* mDeferredPipeline;

  FullscreenRenderPassRef mFullscreenRenderPass;
  FullscreenPipelineRef mFullscreenPipeline;

  FramebufferAttachmentRef mColorAtt;
  DepthBufferAttachmentRef mDepthAtt;
  vk::Framebuffer mGBuffer;

  vk::CommandBuffer mDeferredCB;

  // Helpers
  void drawDeferred(Rendering::Camera& camera, const SceneRef& scene);
};

using DeferredRendererRef = std::shared_ptr<DeferredRenderer>;
}  // namespace Vulkan
}  // namespace Rendering
