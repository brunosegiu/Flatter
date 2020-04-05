#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {
class DepthBuffer {
 public:
  DepthBuffer(const ContextRef& context, const vk::Extent2D& extent);

  const vk::ImageView& getImageView() const { return mImageView; };

  virtual ~DepthBuffer();

 private:
  DepthBuffer(DepthBuffer const&) = delete;
  DepthBuffer& operator=(DepthBuffer const&) = delete;

  vk::Image mImage;
  vk::DeviceMemory mMemory;
  vk::ImageView mImageView;

  const ContextRef mContext;
};

using DepthBufferRef = std::shared_ptr<DepthBuffer>;
}  // namespace Vulkan
}  // namespace Rendering
#pragma once
