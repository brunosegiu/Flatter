#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <string>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class Shader {
 public:
  Shader(const DeviceRef device, const std::vector<unsigned int>& code);
  const VkShaderModule& getNativeHandle() const { return mShaderModuleHandle; }
  virtual ~Shader();

  static const ShaderRef fromFile(const std::string path);

 private:
  VkShaderModule mShaderModuleHandle;
  DeviceRef mDevice;
};

using ShaderRef = std::shared_ptr<Shader>;

}  // namespace Vulkan
}  // namespace Rendering
