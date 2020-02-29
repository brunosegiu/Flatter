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
  Pipeline(const DeviceRef& device, const RenderPass& renderPass);
  void bind(const VkCommandBuffer& command);
  virtual ~Pipeline();

 private:
  Shader* mVertexShader;
  Shader* mFragmentShader;
  VkPipeline mPipelineHandle;
  VkPipelineLayout mPipelineLayoutHandle;
  DeviceRef mDevice;
};

using PipelineRef = std::shared_ptr<Pipeline>;

}  // namespace Vulkan
}  // namespace Rendering
