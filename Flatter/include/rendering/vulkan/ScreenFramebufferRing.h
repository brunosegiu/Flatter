#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Framebuffer.h"
#include "rendering/vulkan/core/Pipeline.h"
#include "rendering/vulkan/core/RenderPass.h"
#include "rendering/vulkan/core/Surface.h"
#include "rendering/vulkan/core/Swapchain.h"
#include "rendering/vulkan/core/Uniform.h"

namespace Rendering {
namespace Vulkan {

const unsigned int FRAMES_IN_FLIGHT_COUNT = 2;

using InFlightFrameResources = struct {
  vk::Fence frameFenceHandle;
  vk::Semaphore availableImageSemaphore;
  vk::Semaphore finishedRenderSemaphore;
};

using CommandBufferResources = struct {
  vk::CommandBuffer commandBuffer;
  FramebufferRef framebuffer;
  UniformMatrixRef matrixUniform;
};

using RenderingResources = struct RenderingResources {
  const vk::Fence& frameFenceHandle;
  const vk::Semaphore& availableImageSemaphore;
  const vk::Semaphore& finishedRenderSemaphore;
  const vk::CommandBuffer& commandBuffer;
  const FramebufferRef& framebuffer;
  const UniformMatrixRef& matrixUniform;
  const unsigned int imageIndex;

  RenderingResources(const vk::Fence& in_frameFenceHandle,
                     const vk::Semaphore& in_availableImageSemaphore,
                     const vk::Semaphore& in_fin_ishedRenderSemaphore,
                     const vk::CommandBuffer& in_commandBuffer,
                     const FramebufferRef& in_framebuffer,
                     const UniformMatrixRef& in_matrixUniform,
                     const unsigned int in_imageIndex)
      : frameFenceHandle(in_frameFenceHandle),
        availableImageSemaphore(in_availableImageSemaphore),
        finishedRenderSemaphore(in_fin_ishedRenderSemaphore),
        commandBuffer(in_commandBuffer),
        framebuffer(in_framebuffer),
        matrixUniform(in_matrixUniform),
        imageIndex(in_imageIndex) {}
};

class ScreenFramebufferRing {
 public:
  ScreenFramebufferRing(const ContextRef& context,
                        const SurfaceRef& surface,
                        const RenderPassRef& renderPass,
                        const VkDescriptorSetLayout& descriptorSetLayout);

  const RenderingResources cycle();

  virtual ~ScreenFramebufferRing();

 private:
  ContextRef mContext;
  SurfaceRef mSurface;
  vk::Format mSurfaceFormat;

  unsigned int mCurrentFrameIndex;
  unsigned int mCurrentImageIndex;
  unsigned int mSwapchainImageCount;

  std::vector<InFlightFrameResources> mInFlightFrameResources;
  std::vector<CommandBufferResources> mCommandBufferResources;

  // Initializers
  void initInFlightFrameResources();

  void initCommandResources(const RenderPassRef& renderPass,
                            const VkDescriptorSetLayout& descriptorSetLayout);
};

using ScreenFramebufferRingRef = std::shared_ptr<ScreenFramebufferRing>;
}  // namespace Vulkan
}  // namespace Rendering
