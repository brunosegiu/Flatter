#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <utility>
#include <vector>

#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/Surface.h"

namespace Rendering {
namespace Vulkan {
const int MAX_DESC_SETS = 256;

class Surface;
using SurfaceRef = std::shared_ptr<Surface>;

class Device {
 public:
  Device(const InstanceRef& instance, const SurfaceRef& surface);

  void waitIdle();

  void allocBuffer(VkDeviceSize size,
                   VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkBuffer& buffer,
                   VkDeviceMemory& bufferMemory) const;

  const VkDevice& getHandle() const { return mDeviceHandle; };
  const VkPhysicalDevice& getPhysicalHandle() const {
    return mPhysicalDeviceHandle;
  };
  const unsigned int getQueueFamilyIndex() const { return mQueueFamilyIndex; };
  const VkQueue& getQueueHandle() const { return mQueueHandle; };

  const VkDescriptorPool& getDescriptorPool() const {
    return mDescriptorPoolHandle;
  };
  const VkCommandPool& getCommandPool() const { return mCommandPoolHandle; };

  virtual ~Device();

 private:
  VkPhysicalDevice mPhysicalDeviceHandle;
  VkDevice mDeviceHandle;

  unsigned int mQueueFamilyIndex;
  VkQueue mQueueHandle;

  VkDescriptorPool mDescriptorPoolHandle;
  VkCommandPool mCommandPoolHandle;

  using PhysicalDeviceInfo =
      const std::pair<const VkPhysicalDevice, const unsigned int>;
  int findQueueFamily(
      const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
      const VkPhysicalDevice& physicalDeviceHandle,
      const VkSurfaceKHR& surfaceHandle);
  PhysicalDeviceInfo findPhysicalDevice(
      const std::vector<VkPhysicalDevice>& devices,
      const VkSurfaceKHR& surfaceHandle);
  unsigned int findBufferMemoryType(
      unsigned int memoryTypeMask,
      VkMemoryPropertyFlags requiredPropertyFlags) const;
};

using DeviceRef = std::shared_ptr<Device>;

}  // namespace Vulkan
}  // namespace Rendering
