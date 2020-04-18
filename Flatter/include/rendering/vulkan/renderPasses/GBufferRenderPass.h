﻿#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/renderPasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {

class GBufferRenderPass : public RenderPass {
 public:
  GBufferRenderPass(const ContextRef& context,
                    const vk::Format& colorFormat,
                    const vk::Format& depthFormat);

  virtual ~GBufferRenderPass();

 private:
  GBufferRenderPass(GBufferRenderPass const&) = delete;
  GBufferRenderPass& operator=(GBufferRenderPass const&) = delete;
};

using GBufferRenderPassRef = std::shared_ptr<GBufferRenderPass>;

}  // namespace Vulkan
}  // namespace Rendering