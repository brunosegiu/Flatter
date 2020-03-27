#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Rendering {
namespace Vulkan {

class Instance : public vk::Instance {
 public:
  Instance();
  const std::vector<vk::PhysicalDevice> getAvailablePhysicalDevices() const;
  virtual ~Instance();

 private:
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  VulkanReportFunc(VkDebugReportFlagsEXT flags,
                   VkDebugReportObjectTypeEXT objType,
                   uint64_t obj,
                   size_t location,
                   int32_t code,
                   const char* layerPrefix,
                   const char* msg,
                   void* userData);

  PFN_vkCreateDebugReportCallbackEXT vkpfn_CreateDebugReportCallbackEXT = 0;
  PFN_vkDestroyDebugReportCallbackEXT vkpfn_DestroyDebugReportCallbackEXT = 0;
  VkDebugReportCallbackEXT debugCallback = VK_NULL_HANDLE;
#endif
};

using InstanceRef = std::shared_ptr<Instance>;

}  // namespace Vulkan
}  // namespace Rendering
