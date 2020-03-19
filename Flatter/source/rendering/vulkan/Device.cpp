#include "rendering/vulkan/Device.h"

#include <assert.h>

#include <vector>

using namespace Rendering::Vulkan;

Device::Device(const InstanceRef& instance, const SurfaceRef& surface) {
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  const std::vector<const char*> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.enabledExtensionCount =
      static_cast<unsigned int>(extensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

  const std::vector<VkPhysicalDevice> physicalDeviceHandles =
      instance->getAvailablePhisicalDevices();

  Device::PhysicalDeviceInfo deviceInfo =
      this->findPhysicalDevice(physicalDeviceHandles, surface->mSurfaceHandle);
  assert(deviceInfo.first != nullptr);
  mPhysicalDeviceHandle = deviceInfo.first;
  mQueueFamilyIndex = deviceInfo.second;

  VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
  deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfo.queueFamilyIndex = this->mQueueFamilyIndex;
  deviceQueueCreateInfo.queueCount = 1;
  const std::vector<float> priorities{1.0f};
  deviceQueueCreateInfo.pQueuePriorities = priorities.data();
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
  VkResult result = vkCreateDevice(mPhysicalDeviceHandle, &deviceCreateInfo,
                                   nullptr, &mDeviceHandle);
  vkGetDeviceQueue(mDeviceHandle, mQueueFamilyIndex, 0, &mQueueHandle);
  assert(result == VK_SUCCESS);

  // Uniform descriptor pool
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = MAX_DESC_SETS;

  VkDescriptorPoolCreateInfo poolCreateInfo{};
  poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolCreateInfo.poolSizeCount = 1;
  poolCreateInfo.pPoolSizes = &poolSize;
  poolCreateInfo.maxSets = MAX_DESC_SETS;

  vkCreateDescriptorPool(mDeviceHandle, &poolCreateInfo, nullptr,
                         &mDescriptorPoolHandle);

  // Command pool
  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = getQueueFamilyIndex();

  vkCreateCommandPool(mDeviceHandle, &commandPoolCreateInfo, 0,
                      &mCommandPoolHandle);
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
  const unsigned int queueFamilyPropertiesSize =
      static_cast<unsigned int>(queueFamilyProperties.size());
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

unsigned int Device::findBufferMemoryType(
    unsigned int memoryTypeMask,
    VkMemoryPropertyFlags requiredPropertyFlags) const {
  VkPhysicalDeviceMemoryProperties properties{};
  vkGetPhysicalDeviceMemoryProperties(mPhysicalDeviceHandle, &properties);

  for (unsigned int memTypeIndex = 0; memTypeIndex < properties.memoryTypeCount;
       memTypeIndex++) {
    const bool filtered = (memoryTypeMask & (1 << memTypeIndex));
    const VkMemoryPropertyFlags memoryFlags =
        properties.memoryTypes[memTypeIndex].propertyFlags;
    if (filtered &&
        (memoryFlags & requiredPropertyFlags) == requiredPropertyFlags) {
      return memTypeIndex;
    }
  }
  return -1;
}

void Device::allocBuffer(VkDeviceSize size,
                         VkBufferUsageFlags usage,
                         VkMemoryPropertyFlags properties,
                         VkBuffer& buffer,
                         VkDeviceMemory& bufferMemory) const {
  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = usage;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  vkCreateBuffer(mDeviceHandle, &bufferCreateInfo, nullptr, &buffer);

  VkMemoryRequirements memRequirements{};
  vkGetBufferMemoryRequirements(mDeviceHandle, buffer, &memRequirements);

  VkMemoryAllocateInfo allocCreateInfo = {};
  allocCreateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocCreateInfo.allocationSize = memRequirements.size;
  allocCreateInfo.memoryTypeIndex =
      findBufferMemoryType(memRequirements.memoryTypeBits, properties);

  vkAllocateMemory(mDeviceHandle, &allocCreateInfo, nullptr, &bufferMemory);

  vkBindBufferMemory(mDeviceHandle, buffer, bufferMemory, 0);
}

void Device::waitIdle() {
  vkDeviceWaitIdle(mDeviceHandle);
}

Device::~Device() {
  vkDestroyDescriptorPool(mDeviceHandle, mDescriptorPoolHandle, nullptr);
  vkDestroyCommandPool(mDeviceHandle, mCommandPoolHandle, 0);
  vkDestroyDevice(mDeviceHandle, 0);
}
