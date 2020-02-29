#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <utility>
#include <vector>

#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/Surface.h"

namespace Rendering {
namespace Vulkan {
class Surface;

class Device {
 public:
  VkPhysicalDevice mPhysicalDeviceHandle;
  VkDevice mDeviceHandle;
  unsigned int mQueueFamilyIndex;
  VkQueue mQueueHandle;

  Device(const Instance& instance, const Surface& surface);
  ~Device();

 private:
  using PhysicalDeviceInfo =
      const std::pair<const VkPhysicalDevice, const unsigned int>;
  int findQueueFamily(
      const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
      const VkPhysicalDevice& physicalDeviceHandle,
      const VkSurfaceKHR& surfaceHandle);
  PhysicalDeviceInfo findPhysicalDevice(
      const std::vector<VkPhysicalDevice>& devices,
      const VkSurfaceKHR& surfaceHandle);
};

}  // namespace Vulkan
}  // namespace Rendering
