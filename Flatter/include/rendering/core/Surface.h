#pragma once

#include <SDL2/SDL_syswm.h>

#include <optional>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Instance.h"

namespace Rendering {
namespace Vulkan {
class Surface {
 public:
  vk::SurfaceKHR mSurfaceHandle;

  unsigned int mWidth, mHeight;

  Surface(const SDL_SysWMinfo& info,
          const InstanceRef& instance,
          const unsigned int width,
          const unsigned int height);
  const vk::SurfaceCapabilitiesKHR getCapabilities(
      const vk::PhysicalDevice& physicalDevice) const;
  const vk::SurfaceFormatKHR& getFormat(
      const vk::PhysicalDevice& physicalDevice) const;

  virtual ~Surface();

 private:
  InstanceRef mInstance;
  mutable std::optional<vk::SurfaceFormatKHR> mSurfaceFormat;
};

using SurfaceRef = std::shared_ptr<Surface>;
}  // namespace Vulkan
}  // namespace Rendering
