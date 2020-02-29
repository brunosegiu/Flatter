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
  unsigned int mFrameCount;
  unsigned int mCurrentFrameIndex;

  VkCommandPool mCommandPoolHandle;

  std::vector<VkCommandBuffer> mCommandBufferHandles;
  std::vector<VkFence> mFrameFenceHandles;
  std::vector<VkSemaphore> mAvailableImageSemaphore;
  std::vector<VkSemaphore> mFinishedRenderSemaphore;

  RenderPass* mRenderPass;
  std::vector<Framebuffer*> mFramebuffers;
  Pipeline* mPipeline;

  const Swapchain* mSwapchain;

  Renderer(const DeviceRef& device,
           Surface& surface,
           const Swapchain& swapchain,
           const unsigned int frameCount);
  void draw();
  virtual ~Renderer();

 private:
  const DeviceRef mDevice;
};

using RendererRef = std::shared_ptr<Renderer>;

}  // namespace Vulkan
}  // namespace Rendering
