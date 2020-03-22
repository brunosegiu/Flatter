#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/Device.h"

namespace Rendering {
namespace Vulkan {

class RenderPass {
 public:
  RenderPass(const SingleDeviceRef& device, const vk::Format& surfaceFormat);

  const vk::RenderPass& getHandle() const { return mRenderPassHandle; };

  virtual ~RenderPass();

 private:
  vk::RenderPass mRenderPassHandle;

  SingleDeviceRef mDevice;
};

using RenderPassRef = std::shared_ptr<RenderPass>;

}  // namespace Vulkan
}  // namespace Rendering
