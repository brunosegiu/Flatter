#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//core/Shader.h"
#include "rendering//pipelines/Pipeline.h"
#include "rendering//renderPasses/GBufferRenderPass.h"
#include "rendering//uniforms/DescriptorLayout.h"

namespace Rendering {
namespace Vulkan {
class DeferredPipeline : public Pipeline {
 public:
  DeferredPipeline(const ContextRef& context,
                   const DescriptorLayoutRef& descriptorLayout,
                   const vk::Format& albedoFormat,
                   const vk::Format& positionFormat,
                   const vk::Format& normalFormat,
                   const vk::Format& depthFormat);

  virtual ~DeferredPipeline();

  enum class Descriptors { ModelViewProjectionMatrix };

 private:
  DeferredPipeline(DeferredPipeline const&) = delete;
  DeferredPipeline& operator=(DeferredPipeline const&) = delete;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;
};

using DeferredPipelineRef = std::shared_ptr<DeferredPipeline>;
}  // namespace Vulkan
}  // namespace Rendering
