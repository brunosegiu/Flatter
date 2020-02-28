#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Shader.h"

namespace Rendering {
namespace Vulkan {

class Pipeline {
 public:
  Pipeline();
  void bind(const DeviceRef device, const VkCommandBuffer& buffer);
  virtual ~Pipeline();

 private:
  VkPipeline mPipelineHandle;

  DeviceRef mDevice;

  ShaderRef mVertexShader;
  ShaderRef mFragmentShader;
};

using PipelineRef = std::shared_ptr<Pipeline>;

}  // namespace Vulkan
}  // namespace Rendering
