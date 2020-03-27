﻿#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Instance.h"
#include "rendering/vulkan/core/Surface.h"
#include "rendering/vulkan/core/Swapchain.h"

namespace Rendering {
namespace Vulkan {
const int MAX_DESC_SETS = 256;

class Surface;
using SurfaceRef = std::shared_ptr<Surface>;

class Context {
 public:
  Context(const InstanceRef& instance, const SurfaceRef& surface);

  void allocBuffer(vk::DeviceSize size,
                   vk::BufferUsageFlags usage,
                   vk::MemoryPropertyFlags properties,
                   vk::Buffer& buffer,
                   vk::DeviceMemory& bufferMemory) const;

  const vk::Device& getDevice() const { return mDevice; };
  const vk::PhysicalDevice& getPhysicalDevice() const {
    return mPhysicalDevice;
  };
  const unsigned int getQueueFamilyIndex() const { return mQueueFamilyIndex; };
  const vk::Queue& getQueue() const { return mQueue; };
  const SwapchainRef& getSwapchain() const { return mSwapchain; };

  const vk::DescriptorPool& getDescriptorPool() const {
    return mDescriptorPoolHandle;
  };
  const vk::CommandPool& getCommandPool() const { return mCommandPoolHandle; };

  virtual ~Context();

 private:
  // Member handles
  vk::Device mDevice;
  vk::PhysicalDevice mPhysicalDevice;
  vk::Queue mQueue;
  unsigned int mQueueFamilyIndex;
  SwapchainRef mSwapchain;

  SurfaceRef mSurface;

  // Allocators and pools
  vk::DescriptorPool mDescriptorPoolHandle;
  vk::CommandPool mCommandPoolHandle;

  using PhysicalDeviceInfo =
      const std::pair<std::optional<vk::PhysicalDevice>, const unsigned int>;

  // Helpers
  int findQueueFamily(
      const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties,
      const vk::PhysicalDevice& physicalDeviceHandle,
      const vk::SurfaceKHR& surfaceHandle);
  PhysicalDeviceInfo findPhysicalDevice(
      const std::vector<vk::PhysicalDevice>& devices,
      const vk::SurfaceKHR& surfaceHandle);
  unsigned int findBufferMemoryType(
      unsigned int memoryTypeMask,
      vk::MemoryPropertyFlags requiredPropertyFlags) const;
};

using ContextRef = std::shared_ptr<Context>;

}  // namespace Vulkan
}  // namespace Rendering