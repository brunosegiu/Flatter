#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//core/Shader.h"
#include "rendering//pipelines/Pipeline.h"
#include "rendering//renderPasses/SingleRenderPass.h"
#include "rendering//uniforms/DescriptorLayout.h"

namespace Rendering {
namespace Vulkan {
class SinglePassPipeline : public Pipeline {
 public:
  SinglePassPipeline(const ContextRef& context,
                     const SingleRenderPassRef& renderPass,
                     const DescriptorLayoutRef& DescriptorLayout);

  virtual ~SinglePassPipeline();

 private:
  SinglePassPipeline(SinglePassPipeline const&) = delete;
  SinglePassPipeline& operator=(SinglePassPipeline const&) = delete;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;
};

using SinglePassPipelineRef = std::shared_ptr<SinglePassPipeline>;
}  // namespace Vulkan
}  // namespace Rendering
