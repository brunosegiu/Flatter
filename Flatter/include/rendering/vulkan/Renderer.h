#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Framebuffer.h"
#include "rendering/vulkan/Pipeline.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"

namespace Rendering {
namespace Vulkan {

class Renderer {
 public:
  Renderer(const DeviceRef& device,
           const SurfaceRef& surface,
           const SwapchainRef& swapchain,
           const unsigned int frameCount);
  void draw();
  virtual ~Renderer();

 private:
  unsigned int mFrameCount;
  unsigned int mCurrentFrameIndex;

  VkCommandPool mCommandPoolHandle;

  std::vector<VkCommandBuffer> mCommandBufferHandles;
  std::vector<VkFence> mFrameFenceHandles;
  std::vector<VkSemaphore> mAvailableImageSemaphore;
  std::vector<VkSemaphore> mFinishedRenderSemaphore;

  RenderPassRef mRenderPass;
  std::vector<FramebufferRef> mFramebuffers;
  PipelineRef mPipeline;
  const DeviceRef mDevice;
  const SwapchainRef mSwapchain;
};

using RendererRef = std::shared_ptr<Renderer>;

}  // namespace Vulkan
}  // namespace Rendering
