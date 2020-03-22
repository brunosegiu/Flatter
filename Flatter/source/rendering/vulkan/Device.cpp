#include "rendering/vulkan/Device.h"

#include <assert.h>

#include <vector>

using namespace Rendering::Vulkan;

SingleDevice::SingleDevice(const InstanceRef& instance,
                           const SurfaceRef& surface) {
  const std::vector<const char*> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  auto deviceCreateInfo = vk::DeviceCreateInfo()
                              .setEnabledExtensionCount(
                                  static_cast<unsigned int>(extensions.size()))
                              .setPpEnabledExtensionNames(extensions.data());

  const std::vector<vk::PhysicalDevice> physicalDeviceHandles =
      instance->getAvailablePhysicalDevices();

  SingleDevice::PhysicalDeviceInfo deviceInfo =
      this->findPhysicalDevice(physicalDeviceHandles, surface->mSurfaceHandle);
  assert(deviceInfo.first.has_value());
  mPhysicalDeviceHandle = deviceInfo.first.value();
  mQueueFamilyIndex = deviceInfo.second;

  const std::vector<float> priorities{1.0f};
  auto const deviceQueueCreateInfo =
      vk::DeviceQueueCreateInfo()
          .setQueueFamilyIndex(this->mQueueFamilyIndex)
          .setQueueCount(1)
          .setPQueuePriorities(priorities.data());
  deviceCreateInfo.setQueueCreateInfoCount(1).setPQueueCreateInfos(
      &deviceQueueCreateInfo);
  assert(mPhysicalDeviceHandle.createDevice(&deviceCreateInfo, nullptr, this) ==
         vk::Result::eSuccess);
  this->getQueue(mQueueFamilyIndex, 0, &mQueueHandle);

  // Uniform descriptor pool
  vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer,
                                  MAX_DESC_SETS);
  auto const poolCreateInfo = vk::DescriptorPoolCreateInfo()
                                  .setPoolSizeCount(1)
                                  .setPPoolSizes(&poolSize)
                                  .setMaxSets(MAX_DESC_SETS);

  this->createDescriptorPool(&poolCreateInfo, nullptr, &mDescriptorPoolHandle);

  // Command pool
  auto const commandPoolCreateInfo =
      vk::CommandPoolCreateInfo()
          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
          .setQueueFamilyIndex(getQueueFamilyIndex());

  this->createCommandPool(&commandPoolCreateInfo, nullptr, &mCommandPoolHandle);
}

SingleDevice::PhysicalDeviceInfo SingleDevice::findPhysicalDevice(
    const std::vector<vk::PhysicalDevice>& devices,
    const vk::SurfaceKHR& surfaceHandle) {
  for (const vk::PhysicalDevice& physicalDeviceHandle : devices) {
    unsigned int queueFamilyCount = 0;
    physicalDeviceHandle.getQueueFamilyProperties(
        &queueFamilyCount, static_cast<vk::QueueFamilyProperties*>(nullptr));
    std::vector<vk::QueueFamilyProperties> deviceQueueFamilyProperties{
        queueFamilyCount, vk::QueueFamilyProperties{}};
    physicalDeviceHandle.getQueueFamilyProperties(
        &queueFamilyCount, deviceQueueFamilyProperties.data());
    const int queueIndex = findQueueFamily(deviceQueueFamilyProperties,
                                           physicalDeviceHandle, surfaceHandle);
    if (queueIndex >= 0) {
      return SingleDevice::PhysicalDeviceInfo(
          physicalDeviceHandle, static_cast<unsigned int>(queueIndex));
    }
  }
  return SingleDevice::PhysicalDeviceInfo(nullptr, -1);
}

int SingleDevice::findQueueFamily(
    const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties,
    const vk::PhysicalDevice& physicalDeviceHandle,
    const vk::SurfaceKHR& surfaceHandle) {
  const unsigned int queueFamilyPropertiesSize =
      static_cast<unsigned int>(queueFamilyProperties.size());
  for (unsigned int queueIndex = 0; queueIndex < queueFamilyPropertiesSize;
       ++queueIndex) {
    const vk::QueueFamilyProperties& properties =
        queueFamilyProperties[queueIndex];
    vk::Bool32 supportsPresent = VK_FALSE;
    physicalDeviceHandle.getSurfaceSupportKHR(queueIndex, surfaceHandle,
                                              &supportsPresent);
    if (supportsPresent &&
        (properties.queueFlags & vk::QueueFlagBits::eGraphics)) {
      return queueIndex;
    }
  }
  return -1;
}

unsigned int SingleDevice::findBufferMemoryType(
    unsigned int memoryTypeMask,
    vk::MemoryPropertyFlags requiredPropertyFlags) const {
  vk::PhysicalDeviceMemoryProperties properties{};
  mPhysicalDeviceHandle.getMemoryProperties(&properties);

  for (unsigned int memTypeIndex = 0; memTypeIndex < properties.memoryTypeCount;
       memTypeIndex++) {
    const bool filtered = (memoryTypeMask & (1 << memTypeIndex));
    const vk::MemoryPropertyFlags memoryFlags =
        properties.memoryTypes[memTypeIndex].propertyFlags;
    if (filtered &&
        (memoryFlags & requiredPropertyFlags) == requiredPropertyFlags) {
      return memTypeIndex;
    }
  }
  return -1;
}

void SingleDevice::allocBuffer(vk::DeviceSize size,
                               vk::BufferUsageFlags usage,
                               vk::MemoryPropertyFlags properties,
                               vk::Buffer& buffer,
                               vk::DeviceMemory& bufferMemory) const {
  auto const bufferCreateInfo =
      vk::BufferCreateInfo().setSize(size).setUsage(usage).setSharingMode(
          vk::SharingMode::eExclusive);
  this->createBuffer(&bufferCreateInfo, nullptr, &buffer);

  vk::MemoryRequirements memRequirements{};
  this->getBufferMemoryRequirements(buffer, &memRequirements);

  auto const allocCreateInfo =
      vk::MemoryAllocateInfo()
          .setAllocationSize(memRequirements.size)
          .setMemoryTypeIndex(
              findBufferMemoryType(memRequirements.memoryTypeBits, properties));

  this->allocateMemory(&allocCreateInfo, nullptr, &bufferMemory);

  this->bindBufferMemory(buffer, bufferMemory, 0);
}

SingleDevice::~SingleDevice() {
  this->destroyDescriptorPool(mDescriptorPoolHandle, nullptr);
  this->destroyCommandPool(mCommandPoolHandle, nullptr);
}
