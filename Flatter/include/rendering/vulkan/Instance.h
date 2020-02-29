#pragma once

#include "rendering/vulkan/Configuration.h"
// Configuration must be included first
#include <assert.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Rendering {
namespace Vulkan {

class Instance {
 public:
  Instance();
  const std::vector<VkPhysicalDevice> getAvailablePhisicalDevices() const;
  const VkInstance& getHandle() const { return mInstanceHandle; };
  virtual ~Instance();

 private:
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
};

using InstanceRef = std::shared_ptr<Instance>;

}  // namespace Vulkan
}  // namespace Rendering
