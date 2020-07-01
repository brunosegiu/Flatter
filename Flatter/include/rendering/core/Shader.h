#pragma once

#include <SDL2/SDL_syswm.h>
#include <stdarg.h>

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"

namespace Rendering {
namespace Vulkan {
class Shader;
using ShaderRef = std::shared_ptr<Shader>;

class Shader {
 public:
  static Rendering::Vulkan::ShaderRef fromFile(
      const std::string& path,
      const ContextRef& context,
      const vk::ShaderStageFlagBits stage);
  Shader(const ContextRef& context,
         const std::vector<char>& code,
         const vk::ShaderStageFlagBits stage);

  const vk::PipelineShaderStageCreateInfo getStageInfo();
  const vk::ShaderModule& getHandle() const { return mShaderHandle; };

  virtual ~Shader();

 private:
  const ContextRef mContext;
  vk::ShaderModule mShaderHandle;
  vk::ShaderStageFlagBits mStage;
};
}  // namespace Vulkan
}  // namespace Rendering
