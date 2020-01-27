#pragma once

#include <vulkan/vulkan.h>

namespace Rendering {
namespace Vulkan {

class SwapChain {
 public:
  SwapChain() = default;
  virtual ~SwapChain() = default;

 private:
  SwapChain(const SwapChain&) = delete;
};

}  // namespace Vulkan
}  // namespace Rendering