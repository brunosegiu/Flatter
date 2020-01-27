#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Queue.h"

namespace Rendering {
namespace Vulkan {

class Instance;

class Device {
 public:
  Device(const Rendering::Vulkan::Instance& vkInstance);
  const std::vector<VkQueueFamilyProperties> getAvailableQueueFamilies();
  const VkPhysicalDevice& getPhysicalDeviceHandle() const {
    return mPhysicalDevice;
  };
  const VkDevice& getDeviceHandle() const { return mDevice; };
  virtual ~Device();

 private:
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDevice mDevice;
  Queue mQueue;
};

}  // namespace Vulkan
}  // namespace Rendering