#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Shader.h"
#include "rendering/vulkan/pipelines/Pipeline.h"
#include "rendering/vulkan/renderPasses/GBufferRenderPass.h"

namespace Rendering {
namespace Vulkan {
class DeferredPipeline : public Pipeline {
 public:
  DeferredPipeline(const ContextRef& context,
                   const GBufferRenderPassRef& renderPass);

  virtual ~DeferredPipeline();

 private:
  DeferredPipeline(DeferredPipeline const&) = delete;
  DeferredPipeline& operator=(DeferredPipeline const&) = delete;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;
};

using DeferredPipelineRef = std::shared_ptr<DeferredPipeline>;
}  // namespace Vulkan
}  // namespace Rendering
