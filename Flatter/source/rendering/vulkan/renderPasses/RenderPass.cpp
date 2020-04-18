#include "rendering/vulkan/renderPasses/RenderPass.h"

using namespace Rendering::Vulkan;

RenderPass::RenderPass(const ContextRef& context) : mContext(context) {}

RenderPass::~RenderPass() {
  const vk::Device& device = mContext->getDevice();
  device.destroyRenderPass(mRenderPassHandle, nullptr);
}
