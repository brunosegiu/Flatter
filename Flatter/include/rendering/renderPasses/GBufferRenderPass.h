#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//renderPasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {
class GBufferRenderPass : public RenderPass {
 public:
  GBufferRenderPass(const ContextRef& context,
                    const vk::Format& albedoFormat,
                    const vk::Format& positionFormat,
                    const vk::Format& normalFormat,
                    const vk::Format& depthFormat);

  virtual ~GBufferRenderPass();

 private:
  GBufferRenderPass(GBufferRenderPass const&) = delete;
  GBufferRenderPass& operator=(GBufferRenderPass const&) = delete;
};

using GBufferRenderPassRef = std::shared_ptr<GBufferRenderPass>;
}  // namespace Vulkan
}  // namespace Rendering
