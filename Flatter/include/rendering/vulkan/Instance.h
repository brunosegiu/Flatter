#pragma once

#include "rendering/vulkan/Configuration.h"

#include <string>

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class Instance {
 public:
  Instance(const std::string appName);

  const VkInstance& getInternalInstance() const;

  virtual ~Instance();

 private:
  VkInstance mInstance;
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
