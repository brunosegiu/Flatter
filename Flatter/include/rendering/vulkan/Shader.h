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

class Shader {
 public:
  VkShaderModule mShaderHandle;

  static Shader* fromFile(const std::string& path, const DeviceRef& device);

  Shader(const DeviceRef& device, const unsigned int* code, const size_t size);

  virtual ~Shader();

 private:
  DeviceRef mDevice;
};

}  // namespace Vulkan
}  // namespace Rendering
