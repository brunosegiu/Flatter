#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/DepthBuffer.h"
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
  vk::Fence frameInUseFence;
  vk::Semaphore imageAvailableSemaphore;
  vk::Semaphore imageRenderedSemaphore;
};

using SwapchainImageResources = struct {
  FramebufferRef framebuffer;
  vk::CommandBuffer commandBuffer;

  UniformMatrixRef matrixUniform;
};

using RenderingResources = struct RenderingResources {
  const vk::Fence& frameInUseFence;
  const vk::Semaphore& imageAvailableSemaphore;
  const vk::Semaphore& imageRenderedSemaphore;
  const vk::CommandBuffer& commandBuffer;
  const FramebufferRef& framebuffer;
  const unsigned int imageIndex;

  const UniformMatrixRef& matrixUniform;

  RenderingResources(const vk::Fence& in_frameFenceHandle,
                     const vk::Semaphore& in_availableImageSemaphore,
                     const vk::Semaphore& in_fin_ishedRenderSemaphore,
                     const vk::CommandBuffer& in_commandBuffer,
                     const FramebufferRef& in_framebuffer,
                     const unsigned int in_imageIndex,
                     const UniformMatrixRef& in_matrixUniform)
      : frameInUseFence(in_frameFenceHandle),
        imageAvailableSemaphore(in_availableImageSemaphore),
        imageRenderedSemaphore(in_fin_ishedRenderSemaphore),
        commandBuffer(in_commandBuffer),
        framebuffer(in_framebuffer),
        imageIndex(in_imageIndex),
        matrixUniform(in_matrixUniform) {}
};

class ScreenFramebufferRing {
 public:
  ScreenFramebufferRing(const ContextRef& context,
                        const SurfaceRef& surface,
                        const RenderPassRef& renderPass,
                        const VkDescriptorSetLayout& descriptorSetLayout);

  const RenderingResources swapBuffers();

  virtual ~ScreenFramebufferRing();

 private:
  ContextRef mContext;
  SurfaceRef mSurface;
  DepthBufferRef mDepthBuffer;
  vk::Format mSurfaceFormat;

  unsigned int mCurrentFrameIndex;
  unsigned int mCurrentImageIndex;
  unsigned int mImageCount;

  std::vector<InFlightFrameResources> mInFlightFrameResources;
  std::vector<SwapchainImageResources> mImagesResources;

  // Initializers
  void initInFlightFrameResources();

  void initImagesResources(const RenderPassRef& renderPass,
                           const VkDescriptorSetLayout& descriptorSetLayout);
};

using ScreenFramebufferRingRef = std::shared_ptr<ScreenFramebufferRing>;
}  // namespace Vulkan
}  // namespace Rendering
