#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/FramebufferAttachment.h"
#include "rendering/vulkan/uniforms/DescriptorLayout.h"
#include "rendering/vulkan/uniforms/DescriptorPool.h"

namespace Rendering {
namespace Vulkan {
class Sampler {
 public:
  Sampler(const ContextRef& context,
          const vk::DescriptorSet descriptorSet,
          const FramebufferAttachmentRef& framebufferAttachment,
          const unsigned int binding);

  virtual ~Sampler();

 private:
  Sampler(Sampler const&) = delete;
  Sampler& operator=(Sampler const&) = delete;

  vk::Sampler mSampler;

  ContextRef mContext;
};

using SamplerRef = std::shared_ptr<Sampler>;
}  // namespace Vulkan
}  // namespace Rendering
