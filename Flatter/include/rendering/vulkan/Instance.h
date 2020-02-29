#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "rendering/vulkan/Configuration.h"
#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/SDLSurface.h"

namespace Rendering {
namespace Vulkan {

class Instance {
 public:
  Instance(const std::string appName, const int width, const int height);

  const VkInstance& getNativeHandle() const;
  void setCurrentSurface(const SDLSurfaceRef surface);

  virtual ~Instance();

 private:
  VkInstance mInstanceHandle;
  DeviceRef mDevice;
  SDLSurfaceRef mSurface;
  SwapchainRef mSwapchain;

  unsigned int mWidth, mHeight;

  Instance(const Instance&) = delete;

#ifdef VK_USE_LUNARG_VALIDATION
  VkDebugReportCallbackCreateInfoEXT mDebugCallbackCreate;
  VkDebugReportCallbackEXT mDebugCallback = VK_NULL_HANDLE;

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  Report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
         uint64_t obj, size_t location, int32_t code, const char* layerPrefix,
         const char* msg, void* userData);
#endif
};

}  // namespace Vulkan
}  // namespace Rendering
