#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering//core/Context.h"
#include "rendering//core/DepthBufferAttachment.h"
#include "rendering//core/Surface.h"
#include "rendering//core/Swapchain.h"
#include "rendering//core/SwapchainFramebuffer.h"
#include "rendering//renderpasses/RenderPass.h"

namespace Rendering {
namespace Vulkan {
const unsigned int FRAMES_IN_FLIGHT_COUNT = 2;

using InFlightFrameResources = struct {
  vk::Fence frameInUseFence;
  vk::Semaphore imageAvailableSemaphore;
  vk::Semaphore imageRenderedSemaphore;
};

using SwapchainImageResources = struct {
  SwapchainFramebufferRef framebuffer;
  vk::CommandBuffer commandBuffer;
};

using RenderingResources = struct RenderingResources {
  const vk::Fence& frameInUseFence;
  const vk::Semaphore& imageAvailableSemaphore;
  const vk::Semaphore& imageRenderedSemaphore;
  const vk::CommandBuffer& commandBuffer;
  const SwapchainFramebufferRef& framebuffer;
  const unsigned int imageIndex;

  RenderingResources(const vk::Fence& in_frameFenceHandle,
                     const vk::Semaphore& in_availableImageSemaphore,
                     const vk::Semaphore& in_finishedRenderSemaphore,
                     const vk::CommandBuffer& in_commandBuffer,
                     const SwapchainFramebufferRef& in_framebuffer,
                     const unsigned int in_imageIndex)
      : frameInUseFence(in_frameFenceHandle),
        imageAvailableSemaphore(in_availableImageSemaphore),
        imageRenderedSemaphore(in_finishedRenderSemaphore),
        commandBuffer(in_commandBuffer),
        framebuffer(in_framebuffer),
        imageIndex(in_imageIndex) {}
};

class ScreenFramebufferRing {
 public:
  ScreenFramebufferRing(
      const ContextRef& context,
      const SurfaceRef& surface,
      const RenderPassRef& renderPass,
      const DepthBufferAttachmentRef& depthBufferAtt = nullptr);

  const RenderingResources swapBuffers();

  virtual ~ScreenFramebufferRing();

 private:
  ScreenFramebufferRing(ScreenFramebufferRing const&) = delete;
  ScreenFramebufferRing& operator=(ScreenFramebufferRing const&) = delete;

  ContextRef mContext;
  SurfaceRef mSurface;
  vk::Format mSurfaceFormat;

  unsigned int mCurrentFrameIndex;
  unsigned int mCurrentImageIndex;
  unsigned int mImageCount;

  std::vector<InFlightFrameResources> mInFlightFrameResources;
  std::vector<SwapchainImageResources> mImagesResources;

  // Initializers
  void initInFlightFrameResources();

  void initImagesResources(
      const RenderPassRef& renderPass,
      const DepthBufferAttachmentRef& depthBufferAtt = nullptr);
};

using ScreenFramebufferRingRef = std::shared_ptr<ScreenFramebufferRing>;
}  // namespace Vulkan
}  // namespace Rendering
