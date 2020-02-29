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

  static Shader* fromFile(const std::string& path, const Device& device);

  Shader(unsigned int* code, const size_t size, const Device& device);

  virtual ~Shader();
};

}  // namespace Vulkan
}  // namespace Rendering
