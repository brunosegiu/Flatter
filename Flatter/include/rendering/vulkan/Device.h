#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "rendering/vulkan/Queue.h"
#include "rendering/vulkan/Swapchain.h"

namespace Rendering {
namespace Vulkan {

class Instance;
class Queue;
using QueueRef = std::shared_ptr<Queue>;

class Device {
 public:
  Device(const Rendering::Vulkan::Instance& vkInstance);
  const std::vector<VkQueueFamilyProperties> getAvailableQueueFamilies();
  const VkPhysicalDevice& getPhysicalDeviceHandle() const {
    return mPhysicalDevice;
  };
  const VkDevice& getNativeHandle() const { return mDeviceHandle; };
  const unsigned int getQueueIndex() const { return mQueue->getIndex(); }
  void submitCommand(const VkSubmitInfo& submitInfo, const VkFence& fence);
  void present(const VkPresentInfoKHR& presentInfo) const;
  virtual ~Device();

 private:
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDevice mDeviceHandle;
  QueueRef mQueue;
};

using DeviceRef = std::shared_ptr<Device>;

}  // namespace Vulkan
}  // namespace Rendering
