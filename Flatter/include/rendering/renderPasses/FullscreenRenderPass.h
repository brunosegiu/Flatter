#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//renderPasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {
class FullscreenRenderPass : public RenderPass {
 public:
  FullscreenRenderPass(const ContextRef& context,
                       const vk::Format& colorFormat);

  virtual ~FullscreenRenderPass();

 private:
  FullscreenRenderPass(FullscreenRenderPass const&) = delete;
  FullscreenRenderPass& operator=(FullscreenRenderPass const&) = delete;
};

using FullscreenRenderPassRef = std::shared_ptr<FullscreenRenderPass>;
}  // namespace Vulkan
}  // namespace Rendering
