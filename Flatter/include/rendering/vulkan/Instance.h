#pragma once

#include <string>

#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>

namespace Rendering {
namespace Vulkan {

class Instance {
 public:
  Instance(const std::string appName);

  const VkInstance& getInternalInstance() const;

  virtual ~Instance();

 private:
  VkInstance mInstance;
  Instance(const Instance&) = delete;
};

}  // namespace Vulkan
}  // namespace Rendering
