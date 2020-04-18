#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Shader.h"
#include "rendering/vulkan/pipelines/Pipeline.h"
#include "rendering/vulkan/renderPasses/SingleRenderPass.h"

namespace Rendering {
namespace Vulkan {

class SinglePassPipeline : public Pipeline {
 public:
  SinglePassPipeline(const ContextRef& context,
                     const SingleRenderPassRef& renderPass,
                     const vk::DescriptorSetLayout& descriptorSetLayout);

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
