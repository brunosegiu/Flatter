#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/RenderPass.h"
#include "rendering/vulkan/core/Shader.h"

namespace Rendering {
namespace Vulkan {

class Pipeline {
 public:
  Pipeline(const ContextRef& context,
           const RenderPassRef& renderPass,
           const vk::DescriptorSetLayout& descriptorSetLayout);

  const vk::Pipeline& getHandle() { return mPipelineHandle; };
  const vk::PipelineLayout& getPipelineLayoutHandle() const {
    return mPipelineLayoutHandle;
  };

  virtual ~Pipeline();

 private:
  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mPipelineLayoutHandle;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;

  ContextRef mContext;
};

using PipelineRef = std::shared_ptr<Pipeline>;

}  // namespace Vulkan
}  // namespace Rendering
