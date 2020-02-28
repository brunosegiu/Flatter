#include "rendering/vulkan/Queue.h"

#include <assert.h>

#include <algorithm>

using namespace Rendering::Vulkan;

Queue::Queue(const DeviceRef device, const unsigned int queueIndex)
    : mIndex(queueIndex) {
  vkGetDeviceQueue(device->getNativeHandle(), mIndex, 0, &mQueueHandle);
}

const unsigned int Queue::searchSuitableFamily(
    const std::vector<VkQueueFamilyProperties>& properties) {
  const auto it =
      std::find_if(properties.begin(), properties.end(),
                   [](const VkQueueFamilyProperties& queueProp) {
                     return queueProp.queueFlags & VK_QUEUE_GRAPHICS_BIT;
                   });

  const unsigned int index = static_cast<unsigned int>(it - properties.begin());
  assert(index < properties.size());
  return index;
}

const VkDeviceQueueCreateInfo Queue::getInfo(const unsigned int queueIndex) {
  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = queueIndex;
  queueCreateInfo.queueCount = 1;
  const float priorities[] = {1.0f};
  queueCreateInfo.pQueuePriorities = priorities;
  return queueCreateInfo;
}

void Queue::init(const VkDevice& device) {}

void Queue::submit(const VkSubmitInfo& submitInfo, const VkFence& fence) {
  vkQueueSubmit(mQueueHandle, 1, &submitInfo, fence);
}

void Queue::present(const VkPresentInfoKHR& presentInfo) const {
  vkQueuePresentKHR(mQueueHandle, &presentInfo);
}

Queue::~Queue(){};
