#pragma once

#include "rendering/vulkan/Configuration.h"
// Defines macros so that platform = Win

#include <SDL2/SDL_syswm.h>
#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Instance.h"

namespace Rendering {
namespace Vulkan {
class Device;

class Surface {
 public:
  VkSurfaceKHR mSurfaceHandle;
  VkSurfaceFormatKHR* surfaceFormat;

  unsigned int mWidth, mHeight;

  Surface(const SDL_SysWMinfo& info,
          const Instance& instance,
          const unsigned int width,
          const unsigned int height);
  const VkSurfaceCapabilitiesKHR getCapabilities(const Device& device) const;
  const VkSurfaceFormatKHR getSurfaceFormat(const Device& device);

  virtual ~Surface();
};

}  // namespace Vulkan
}  // namespace Rendering
