#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/Surface.h"

namespace Rendering {
namespace Vulkan {
const int MAX_DESC_SETS = 256;

class Surface;
using SurfaceRef = std::shared_ptr<Surface>;

class SingleDevice : public vk::Device {
 public:
  SingleDevice(const InstanceRef& instance, const SurfaceRef& surface);

  void allocBuffer(vk::DeviceSize size,
                   vk::BufferUsageFlags usage,
                   vk::MemoryPropertyFlags properties,
                   vk::Buffer& buffer,
                   vk::DeviceMemory& bufferMemory) const;

  const vk::PhysicalDevice& getPhysicalHandle() const {
    return mPhysicalDeviceHandle;
  };
  const unsigned int getQueueFamilyIndex() const { return mQueueFamilyIndex; };
  const vk::Queue& getQueueHandle() const { return mQueueHandle; };

  const vk::DescriptorPool& getDescriptorPool() const {
    return mDescriptorPoolHandle;
  };
  const vk::CommandPool& getCommandPool() const { return mCommandPoolHandle; };

  virtual ~SingleDevice();

 private:
  vk::PhysicalDevice mPhysicalDeviceHandle;

  unsigned int mQueueFamilyIndex;
  vk::Queue mQueueHandle;

  vk::DescriptorPool mDescriptorPoolHandle;
  vk::CommandPool mCommandPoolHandle;

  using PhysicalDeviceInfo =
      const std::pair<std::optional<vk::PhysicalDevice>, const unsigned int>;

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

using SingleDeviceRef = std::shared_ptr<SingleDevice>;

}  // namespace Vulkan
}  // namespace Rendering
