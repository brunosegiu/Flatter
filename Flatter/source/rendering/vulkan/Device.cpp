#include "rendering/vulkan/Device.h"

#include <assert.h>
#include <vector>

#include "rendering/vulkan/Instance.h"

using namespace Rendering::Vulkan;

Rendering::Vulkan::Device::Device(const Instance &vkInstance) {
  unsigned int deviceCount = 0;
  vkEnumeratePhysicalDevices(vkInstance.getInternalInstance(), &deviceCount,
                             nullptr);
  assert(deviceCount);
  std::vector<VkPhysicalDevice> vkDevices(deviceCount);
  // Pick phyisical device
  vkEnumeratePhysicalDevices(vkInstance.getInternalInstance(), &deviceCount,
                             vkDevices.data());
  for (const auto &vkDevice : vkDevices) {
    VkPhysicalDeviceProperties vkDeviceProperties;
    vkGetPhysicalDeviceProperties(vkDevice, &vkDeviceProperties);
    if (vkDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      mPhysicalDevice = vkDevice;
      break;
    }
  }

  assert(mPhysicalDevice != VK_NULL_HANDLE);

  // Initialize logical device
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  const std::vector<const char *> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.enabledExtensionCount =
      static_cast<unsigned int>(extensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

  const std::vector<VkQueueFamilyProperties> properties =
      this->getAvailableQueueFamilies();
  mQueue.searchSuitableFamily(properties);
  deviceCreateInfo.pQueueCreateInfos = &mQueue.getInfo();

  VkResult result =
      vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice);

  mQueue.init(mDevice);

  assert(result == VK_SUCCESS);
}

const std::vector<VkQueueFamilyProperties>
Rendering::Vulkan::Device::getAvailableQueueFamilies() {
  unsigned int familyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount,
                                           queueFamilies.data());
  return queueFamilies;
}

Rendering::Vulkan::Device::~Device() {}