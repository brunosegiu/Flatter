#pragma once

#include <vulkan/vulkan.h>

namespace Rendering {
namespace Vulkan {

class Surface {
 public:
  Surface() = default;

  const VkSurfaceKHR& getSurfaceHandle() const { return mSurface; };

  virtual ~Surface() = default;

 protected:
  VkSurfaceKHR mSurface;

 private:
  Surface(const Surface&) = delete;
};

}  // namespace Vulkan
}  // namespace Rendering