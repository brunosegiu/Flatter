#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Shader.h"

namespace Rendering {
namespace Vulkan {

class Pipeline {
 public:
  Shader* mVertexShader;
  Shader* mFragmentShader;
  VkPipeline mPipelineHandle;
  VkPipelineLayout mPipelineLayoutHandle;

  Pipeline(const Device& device, const RenderPass& renderPass);

  virtual ~Pipeline();
};

using PipelineRef = std::shared_ptr<Pipeline>;

}  // namespace Vulkan
}  // namespace Rendering
