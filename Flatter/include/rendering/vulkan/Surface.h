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
using DeviceRef = std::shared_ptr<Device>;

class Surface {
 public:
  VkSurfaceKHR mSurfaceHandle;

  unsigned int mWidth, mHeight;

  Surface(const SDL_SysWMinfo& info,
          const InstanceRef& instance,
          const unsigned int width,
          const unsigned int height);
  const VkSurfaceCapabilitiesKHR getCapabilities(const DeviceRef& device) const;
  const VkSurfaceFormatKHR getSurfaceFormat(const DeviceRef& device);

  virtual ~Surface();

 private:
  InstanceRef mInstance;
  std::unique_ptr<VkSurfaceFormatKHR> mSurfaceFormat;
};

using SurfaceRef = std::shared_ptr<Surface>;

}  // namespace Vulkan
}  // namespace Rendering
