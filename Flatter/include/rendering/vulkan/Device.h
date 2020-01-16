#include <vector>

#include <vulkan/vulkan.h>

#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/Queue.h"

namespace Rendering {
namespace Vulkan {

class Device {
 public:
  Device(const Instance& vkInstance);
  const std::vector<VkQueueFamilyProperties>& getAvailableQueueFamilies();
  virtual ~Device();

 private:
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDevice mVkDevice;
  Queue mQueue;
};

}  // namespace Vulkan
}  // namespace Rendering