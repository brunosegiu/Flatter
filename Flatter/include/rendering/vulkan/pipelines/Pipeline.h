#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/renderpasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {
class Pipeline {
 public:
  Pipeline(const ContextRef& context);

  const vk::Pipeline& getHandle() { return mPipelineHandle; };
  const vk::PipelineLayout& getPipelineLayout() const {
    return mPipelineLayout;
  };
  const RenderPassRef& getRenderPass() const { return mRenderPass; };

  virtual ~Pipeline();

 protected:
  Pipeline(Pipeline const&) = delete;
  Pipeline& operator=(Pipeline const&) = delete;

  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mPipelineLayout;

  ContextRef mContext;
  RenderPassRef mRenderPass;
};

using PipelineRef = std::shared_ptr<Pipeline>;
}  // namespace Vulkan
}  // namespace Rendering
