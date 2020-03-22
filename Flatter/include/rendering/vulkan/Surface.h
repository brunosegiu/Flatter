#pragma once

#include <SDL2/SDL_syswm.h>

#include <utility>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Instance.h"

namespace Rendering {
namespace Vulkan {

class SingleDevice;
using SingleDeviceRef = std::shared_ptr<SingleDevice>;

class Surface {
 public:
  vk::SurfaceKHR mSurfaceHandle;

  unsigned int mWidth, mHeight;

  Surface(const SDL_SysWMinfo& info,
          const InstanceRef& instance,
          const unsigned int width,
          const unsigned int height);
  const vk::SurfaceCapabilitiesKHR getCapabilities(
      const SingleDeviceRef& device) const;
  const vk::SurfaceFormatKHR& getSurfaceFormat(
      const SingleDeviceRef& device) const;

  virtual ~Surface();

 private:
  InstanceRef mInstance;
  mutable std::optional<vk::SurfaceFormatKHR> mSurfaceFormat;
};

using SurfaceRef = std::shared_ptr<Surface>;

}  // namespace Vulkan
}  // namespace Rendering
