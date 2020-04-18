#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {

class RenderPass {
 public:
  RenderPass(const ContextRef& context);

  const vk::RenderPass& getHandle() const { return mRenderPassHandle; };

  virtual ~RenderPass();

 protected:
  RenderPass(RenderPass const&) = delete;
  RenderPass& operator=(RenderPass const&) = delete;

  vk::RenderPass mRenderPassHandle;

  ContextRef mContext;
};

using RenderPassRef = std::shared_ptr<RenderPass>;

}  // namespace Vulkan
}  // namespace Rendering
