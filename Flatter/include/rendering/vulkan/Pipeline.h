#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Shader.h"

namespace Rendering {
namespace Vulkan {

class Pipeline {
 public:
  Pipeline(const DeviceRef& device,
           const RenderPassRef& renderPass,
           const VkDescriptorSetLayout& descriptorSetLayout);

  const VkPipeline& getHandle() { return mPipelineHandle; };
  const VkPipelineLayout& getPipelineLayoutHandle() const {
    return mPipelineLayoutHandle;
  };

  virtual ~Pipeline();

 private:
  VkPipeline mPipelineHandle;
  VkPipelineLayout mPipelineLayoutHandle;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;

  DeviceRef mDevice;
};

using PipelineRef = std::shared_ptr<Pipeline>;

}  // namespace Vulkan
}  // namespace Rendering
