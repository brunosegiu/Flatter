#include "rendering/vulkan/Queue.h"

#include <assert.h>
#include <algorithm>

using namespace Rendering::Vulkan;

Queue::Queue() {}

void Queue::searchSuitableFamily(
    const std::vector<VkQueueFamilyProperties>& properties) {
  const auto it =
      std::find_if(properties.begin(), properties.end(),
                   [](const VkQueueFamilyProperties& queueProp) {
                     return queueProp.queueFlags & VK_QUEUE_GRAPHICS_BIT;
                   });

  const unsigned int index = static_cast<unsigned int>(it - properties.begin());
  assert(mIndex < properties.size());
  mIndex = index;
}

const VkDeviceQueueCreateInfo& Queue::getInfo() const {
  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = mIndex;
  queueCreateInfo.queueCount = 1;
  const float priorities[] = {1.0f};
  queueCreateInfo.pQueuePriorities = priorities;
  return queueCreateInfo;
}

void Queue::init(const VkDevice& device) {
  vkGetDeviceQueue(device, mIndex, 0, &mQueue);
}

Queue::~Queue(){};