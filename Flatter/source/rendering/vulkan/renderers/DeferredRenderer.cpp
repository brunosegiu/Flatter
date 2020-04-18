#include "rendering/vulkan/renderers/DeferredRenderer.h"

using namespace Rendering::Vulkan;

DeferredRenderer::DeferredRenderer(const ContextRef& context,
                                   const SurfaceRef& surface)
    : Renderer(context) {
  const vk::Device& device = mContext->getDevice();

  const vk::Extent2D extent(mContext->getSwapchain()->getExtent());
  mColorAtt = std::make_shared<FramebufferAttachment>(
      mContext, extent, vk::Format::eR8G8B8Unorm,
      vk::ImageUsageFlagBits::eColorAttachment |
          vk::ImageUsageFlagBits::eSampled,
      vk::ImageAspectFlagBits::eColor);
  mDepthAtt = std::make_shared<DepthBufferAttachment>(mContext, extent);
}

void DeferredRenderer::draw(Rendering::Camera& camera, const SceneRef& scene) {}

DeferredRenderer::~DeferredRenderer() {}
