#include "rendering/vulkan/Device.h"

#include <assert.h>
#include <vector>

using namespace Rendering::Vulkan;

Device::Device(const Instance &vkInstance) {
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
  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = 0;
  deviceCreateInfo.enabledLayerCount = 0;
  deviceCreateInfo.ppEnabledLayerNames = 0;
  deviceCreateInfo.enabledExtensionCount = 1;
  const char *pDeviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.ppEnabledExtensionNames = pDeviceExtensions;
  deviceCreateInfo.pEnabledFeatures = 0;

  const std::vector<VkQueueFamilyProperties> properties =
      getAvailableQueueFamilies();
  mQueue.searchSuitableFamily(properties);
  deviceCreateInfo.pQueueCreateInfos = &mQueue.getInfo();

  VkResult result =
      vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mVkDevice);

  mQueue.init(mVkDevice);

  assert(result == VK_SUCCESS);
}

const std::vector<VkQueueFamilyProperties>
    &Device::getAvailableQueueFamilies() {
  unsigned int familyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount,
                                           queueFamilies.data());
  return queueFamilies;
}

Device::~Device() {}