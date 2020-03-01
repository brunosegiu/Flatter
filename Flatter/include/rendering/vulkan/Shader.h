#pragma once

#include <SDL2/SDL_syswm.h>
#include <stdarg.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class Shader;
using ShaderRef = std::shared_ptr<Shader>;

class Shader {
 public:
  static Rendering::Vulkan::ShaderRef fromFile(const std::string& path,
                                               const DeviceRef& device);

  Shader(const DeviceRef& device, const std::vector<char>& code);

  const VkShaderModule& getHandle() const { return mShaderHandle; };

  virtual ~Shader();

 private:
  const DeviceRef mDevice;
  VkShaderModule mShaderHandle;
};

}  // namespace Vulkan
}  // namespace Rendering
