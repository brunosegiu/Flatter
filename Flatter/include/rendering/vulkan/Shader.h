#pragma once

#include <SDL2/SDL_syswm.h>
#include <stdarg.h>

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class Shader;
using ShaderRef = std::shared_ptr<Shader>;

class Shader {
 public:
  static Rendering::Vulkan::ShaderRef fromFile(const std::string& path,
                                               const SingleDeviceRef& device);

  Shader(const SingleDeviceRef& device, const std::vector<char>& code);

  const vk::ShaderModule& getHandle() const { return mShaderHandle; };

  virtual ~Shader();

 private:
  const SingleDeviceRef mDevice;
  vk::ShaderModule mShaderHandle;
};

}  // namespace Vulkan
}  // namespace Rendering
