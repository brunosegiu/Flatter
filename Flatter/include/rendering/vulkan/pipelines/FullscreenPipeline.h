#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Shader.h"
#include "rendering/vulkan/pipelines/Pipeline.h"
#include "rendering/vulkan/renderPasses/FullscreenRenderPass.h"

namespace Rendering {
namespace Vulkan {
class FullscreenPipeline : public Pipeline {
 public:
  FullscreenPipeline(const ContextRef& context,
                     const FullscreenRenderPassRef& renderPass);

  virtual ~FullscreenPipeline();

 private:
  FullscreenPipeline(FullscreenPipeline const&) = delete;
  FullscreenPipeline& operator=(FullscreenPipeline const&) = delete;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;
};

using FullscreenPipelineRef = std::shared_ptr<FullscreenPipeline>;
}  // namespace Vulkan
}  // namespace Rendering
