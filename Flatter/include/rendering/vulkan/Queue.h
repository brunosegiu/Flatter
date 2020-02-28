#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class Device;
using DeviceRef = std::shared_ptr<Device>;

class Queue {
 public:
  static const unsigned int searchSuitableFamily(
      const std::vector<VkQueueFamilyProperties> &properties);
  static const VkDeviceQueueCreateInfo getInfo(const unsigned int queueIndex);

  Queue(const DeviceRef device, const unsigned int queueIndex);
  const unsigned int &getIndex() const { return mIndex; };
  void init(const VkDevice &device);
  void submit(const VkSubmitInfo &submitInfo, const VkFence &fence);
  void present(const VkPresentInfoKHR &presentInfo) const;

  virtual ~Queue();

 private:
  unsigned int mIndex;
  VkQueue mQueueHandle;
  Queue(const Queue &) = delete;
};

using QueueRef = std::shared_ptr<Queue>;

}  // namespace Vulkan
}  // namespace Rendering
