#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace Rendering {
namespace Vulkan {

class Queue {
 public:
  Queue();

  void searchSuitableFamily(
      const std::vector<VkQueueFamilyProperties> &properties);
  const VkDeviceQueueCreateInfo getInfo() const;
  void init(const VkDevice &device);

  virtual ~Queue();

 private:
  unsigned int mIndex;
  VkQueue mQueue;
  Queue(const Queue &) = delete;
};

}  // namespace Vulkan
}  // namespace Rendering