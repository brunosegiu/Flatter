#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Framebuffer.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/SwapChain.h"

namespace Rendering {
namespace Vulkan {

class Renderer {
 public:
  Renderer(const DeviceRef device, const SwapChainRef swapChain,
           const unsigned int frameCount);
  void draw();
  virtual ~Renderer();

 private:
  VkCommandPool mCommandPoolHandle;
  std::vector<VkCommandBuffer> mCommandBufferHandles;
  std::vector<FramebufferRef> mFrameBuffers;
  RenderPassRef mRenderPass;
  SwapChainRef mSwapChain;

  std::vector<VkSemaphore> mRenderSemaphores;
  std::vector<VkFence> mFenceHandles;

  long unsigned int mFrameIndex;
  unsigned int mFrameCount;

  DeviceRef mDevice;
};

using RendererRef = std::shared_ptr<Renderer>;

}  // namespace Vulkan
}  // namespace Rendering
