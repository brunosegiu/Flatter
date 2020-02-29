#include "rendering/vulkan/Device.h"

#include <assert.h>

#include <vector>

using namespace Rendering::Vulkan;

Device::Device(const Instance& instance, const Surface& surface) {
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  const std::vector<const char*> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.enabledExtensionCount = extensions.size();
  deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

  unsigned int physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(instance.mInstanceHandle, &physicalDeviceCount, 0);
  std::vector<VkPhysicalDevice> physicalDeviceHandles(physicalDeviceCount,
                                                      nullptr);
  vkEnumeratePhysicalDevices(instance.mInstanceHandle, &physicalDeviceCount,
                             physicalDeviceHandles.data());

  Device::PhysicalDeviceInfo deviceInfo =
      this->findPhysicalDevice(physicalDeviceHandles, surface.mSurfaceHandle);
  assert(deviceInfo.first != nullptr);
  mPhysicalDeviceHandle = deviceInfo.first;
  mQueueFamilyIndex = deviceInfo.second;

  VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
  deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfo.queueFamilyIndex = this->mQueueFamilyIndex;
  deviceQueueCreateInfo.queueCount = 1;
  deviceQueueCreateInfo.pQueuePriorities = std::vector<float>{1.0f}.data();
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
  VkResult result = vkCreateDevice(mPhysicalDeviceHandle, &deviceCreateInfo,
                                   nullptr, &mDeviceHandle);
  vkGetDeviceQueue(mDeviceHandle, mQueueFamilyIndex, 0, &mQueueHandle);
  assert(result == VK_SUCCESS);
}

Device::PhysicalDeviceInfo Device::findPhysicalDevice(
    const std::vector<VkPhysicalDevice>& devices,
    const VkSurfaceKHR& surfaceHandle) {
  for (const VkPhysicalDevice& physicalDeviceHandle : devices) {
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle,
                                             &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(
        queueFamilyCount, VkQueueFamilyProperties{});
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDeviceHandle, &queueFamilyCount,
        deviceQueueFamilyProperties.data());
    const int queueIndex = findQueueFamily(deviceQueueFamilyProperties,
                                           physicalDeviceHandle, surfaceHandle);
    if (queueIndex >= 0) {
      return Device::PhysicalDeviceInfo(physicalDeviceHandle,
                                        static_cast<unsigned int>(queueIndex));
    }
  }
  return Device::PhysicalDeviceInfo(nullptr, -1);
}

int Device::findQueueFamily(
    const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
    const VkPhysicalDevice& physicalDeviceHandle,
    const VkSurfaceKHR& surfaceHandle) {
  const unsigned int queueFamilyPropertiesSize = queueFamilyProperties.size();
  for (unsigned int queueIndex = 0; queueIndex < queueFamilyPropertiesSize;
       ++queueIndex) {
    const VkQueueFamilyProperties& properties =
        queueFamilyProperties[queueIndex];
    VkBool32 supportsPresent = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDeviceHandle, queueIndex,
                                         surfaceHandle, &supportsPresent);
    if (supportsPresent && (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      return queueIndex;
    }
  }
  return -1;
}

Device::~Device() {
  vkDestroyDevice(mDeviceHandle, 0);
}
