#include "rendering/vulkan/core/Context.h"

#include <assert.h>

#include <vector>

using namespace Rendering::Vulkan;

Context::Context(const InstanceRef& instance, const SurfaceRef& surface)
    : mSurface(surface) {
  const std::vector<const char*> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  auto deviceCreateInfo = vk::DeviceCreateInfo()
                              .setEnabledExtensionCount(
                                  static_cast<unsigned int>(extensions.size()))
                              .setPpEnabledExtensionNames(extensions.data());

  const std::vector<vk::PhysicalDevice> physicalDeviceHandles =
      instance->getAvailablePhysicalDevices();

  Context::PhysicalDeviceInfo deviceInfo =
      this->findPhysicalDevice(physicalDeviceHandles, surface->mSurfaceHandle);
  assert(deviceInfo.first.has_value());
  mPhysicalDevice = deviceInfo.first.value();
  mQueueFamilyIndex = deviceInfo.second;

  const std::vector<float> priorities{1.0f};
  auto const deviceQueueCreateInfo =
      vk::DeviceQueueCreateInfo{}
          .setQueueFamilyIndex(this->mQueueFamilyIndex)
          .setQueueCount(1)
          .setPQueuePriorities(priorities.data());
  deviceCreateInfo.setQueueCreateInfoCount(1).setPQueueCreateInfos(
      &deviceQueueCreateInfo);
  assert(mPhysicalDevice.createDevice(&deviceCreateInfo, nullptr, &mDevice) ==
         vk::Result::eSuccess);
  mDevice.getQueue(mQueueFamilyIndex, 0, &mQueue);

  // Uniform descriptor pool
  vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer,
                                  MAX_DESC_SETS);
  auto const poolCreateInfo = vk::DescriptorPoolCreateInfo{}
                                  .setPoolSizeCount(1)
                                  .setPPoolSizes(&poolSize)
                                  .setMaxSets(MAX_DESC_SETS);

  assert(mDevice.createDescriptorPool(&poolCreateInfo, nullptr,
                                      &mDescriptorPoolHandle) ==
         vk::Result::eSuccess);

  // Command pool
  auto const commandPoolCreateInfo =
      vk::CommandPoolCreateInfo{}
          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
          .setQueueFamilyIndex(getQueueFamilyIndex());

  assert(mDevice.createCommandPool(&commandPoolCreateInfo, nullptr,
                                   &mCommandPoolHandle) ==
         vk::Result::eSuccess);

  // Swapchain
  mSwapchain = std::make_shared<Swapchain>(mDevice, mPhysicalDevice, mSurface);
}

Context::PhysicalDeviceInfo Context::findPhysicalDevice(
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
      return Context::PhysicalDeviceInfo(physicalDeviceHandle,
                                         static_cast<unsigned int>(queueIndex));
    }
  }
  return Context::PhysicalDeviceInfo(nullptr, -1);
}

int Context::findQueueFamily(
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

unsigned int Context::findBufferMemoryType(
    unsigned int memoryTypeMask,
    vk::MemoryPropertyFlags requiredPropertyFlags) const {
  vk::PhysicalDeviceMemoryProperties properties{};
  mPhysicalDevice.getMemoryProperties(&properties);

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

void Context::createBuffer(vk::DeviceSize size,
                           vk::BufferUsageFlags usage,
                           vk::MemoryPropertyFlags properties,
                           vk::Buffer& buffer,
                           vk::DeviceMemory& bufferMemory) const {
  auto const bufferCreateInfo =
      vk::BufferCreateInfo{}.setSize(size).setUsage(usage).setSharingMode(
          vk::SharingMode::eExclusive);
  mDevice.createBuffer(&bufferCreateInfo, nullptr, &buffer);

  vk::MemoryRequirements memRequirements{};
  mDevice.getBufferMemoryRequirements(buffer, &memRequirements);

  auto const allocCreateInfo =
      vk::MemoryAllocateInfo{}
          .setAllocationSize(memRequirements.size)
          .setMemoryTypeIndex(
              findBufferMemoryType(memRequirements.memoryTypeBits, properties));

  assert(mDevice.allocateMemory(&allocCreateInfo, nullptr, &bufferMemory) ==
         vk::Result::eSuccess);

  assert(mDevice.bindBufferMemory(buffer, bufferMemory, 0) ==
         vk::Result::eSuccess);
}

void Context::copyBuffer(vk::Buffer srcBuffer,
                         vk::Buffer dstBuffer,
                         vk::DeviceSize bufferSize) const {
  const auto commandAllocInfo = vk::CommandBufferAllocateInfo{}
                                    .setLevel(vk::CommandBufferLevel::ePrimary)
                                    .setCommandPool(mCommandPoolHandle)
                                    .setCommandBufferCount(1);
  vk::CommandBuffer commandBuffer{};
  mDevice.allocateCommandBuffers(&commandAllocInfo, &commandBuffer);

  const auto commandBeginInfo = vk::CommandBufferBeginInfo{}.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  commandBuffer.begin(&commandBeginInfo);
  const auto region =
      vk::BufferCopy{}.setSrcOffset(0).setDstOffset(0).setSize(bufferSize);
  commandBuffer.copyBuffer(srcBuffer, dstBuffer, region);
  commandBuffer.end();

  const auto commandSubmitInfo =
      vk::SubmitInfo{}.setCommandBufferCount(1).setPCommandBuffers(
          &commandBuffer);
  mQueue.submit(commandSubmitInfo, vk::Fence());
  mQueue.waitIdle();
  mDevice.freeCommandBuffers(mCommandPoolHandle, commandBuffer);
}

const vk::Format Context::findSupportedFormat(
    const std::vector<vk::Format>& candidates,
    const vk::ImageTiling& tiling,
    vk::FormatFeatureFlags& flags) {
  const auto formatIter = std::find_if(
      candidates.begin(), candidates.end(), [&](const vk::Format& format) {
        vk::FormatProperties formatProperties;
        mPhysicalDevice.getFormatProperties(format, &formatProperties);
        return (((tiling == vk::ImageTiling::eLinear &&
                  (formatProperties.linearTilingFeatures & flags) == flags)) ||
                ((tiling == vk::ImageTiling::eOptimal &&
                  (formatProperties.optimalTilingFeatures & flags) == flags)));
      });
  assert(formatIter != candidates.end());
  return *formatIter;
}

Context::~Context() {
  mDevice.destroyDescriptorPool(mDescriptorPoolHandle, nullptr);
  mDevice.destroyCommandPool(mCommandPoolHandle, nullptr);
}
