#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/renderPasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {

class SingleRenderPass : public RenderPass {
 public:
  SingleRenderPass(const ContextRef& context,
                   const vk::Format& surfaceFormat,
                   const vk::Format& depthFormat);

  virtual ~SingleRenderPass();

 private:
  SingleRenderPass(SingleRenderPass const&) = delete;
  SingleRenderPass& operator=(SingleRenderPass const&) = delete;
};

using SingleRenderPassRef = std::shared_ptr<SingleRenderPass>;

}  // namespace Vulkan
}  // namespace Rendering
