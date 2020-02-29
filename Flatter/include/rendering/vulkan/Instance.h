#pragma once

#include "rendering/vulkan/Configuration.h"
// Configuration must be first
#include <assert.h>
#include <vulkan/vulkan.h>

#include <vector>

namespace Rendering {
namespace Vulkan {

class Instance {
 public:
  VkInstance mInstanceHandle;

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  VulkanReportFunc(VkDebugReportFlagsEXT flags,
                   VkDebugReportObjectTypeEXT objType,
                   uint64_t obj,
                   size_t location,
                   int32_t code,
                   const char* layerPrefix,
                   const char* msg,
                   void* userData);
  Instance();
  virtual ~Instance();
};

}  // namespace Vulkan
}  // namespace Rendering
