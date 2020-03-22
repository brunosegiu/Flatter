#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Shader.h"

namespace Rendering {
namespace Vulkan {

class Pipeline {
 public:
  Pipeline(const SingleDeviceRef& device,
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

  SingleDeviceRef mDevice;
};

using PipelineRef = std::shared_ptr<Pipeline>;

}  // namespace Vulkan
}  // namespace Rendering
