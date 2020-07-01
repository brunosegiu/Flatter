#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//core/Shader.h"
#include "rendering//pipelines/Pipeline.h"
#include "rendering//renderPasses/FullscreenRenderPass.h"
#include "rendering//uniforms/DescriptorLayout.h"

namespace Rendering {
namespace Vulkan {
class FullscreenPipeline : public Pipeline {
 public:
  FullscreenPipeline(const ContextRef& context,
                     const std::vector<DescriptorLayoutRef>& descriptorLayouts,
                     const FullscreenRenderPassRef& renderPass,
                     const unsigned int maxLightCount);

  virtual ~FullscreenPipeline();

  enum class Descriptors { AlbedoSampler };

 private:
  FullscreenPipeline(FullscreenPipeline const&) = delete;
  FullscreenPipeline& operator=(FullscreenPipeline const&) = delete;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;
};

using FullscreenPipelineRef = std::shared_ptr<FullscreenPipeline>;
}  // namespace Vulkan
}  // namespace Rendering
