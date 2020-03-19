#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "rendering/Camera.h"
#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Framebuffer.h"
#include "rendering/vulkan/Pipeline.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"
#include "rendering/vulkan/Uniform.h"

namespace Rendering {
namespace Vulkan {

using InFlightFrameResource = struct {
  VkFence frameFenceHandle = VK_NULL_HANDLE;
  VkSemaphore availableImageSemaphore = VK_NULL_HANDLE;
  VkSemaphore finishedRenderSemaphore = VK_NULL_HANDLE;

  UniformMatrixRef matrixUniform = nullptr;
};

using CommandBufferResources = struct {
  VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
  FramebufferRef framebuffer = VK_NULL_HANDLE;
};

const unsigned int FRAMES_IN_FLIGHT_COUNT = 2;

class Renderer {
 public:
  Renderer(const DeviceRef& device,
           const SurfaceRef& surface,
           const SwapchainRef& swapchain);

  void draw(const Rendering::Camera& camera);

  virtual ~Renderer();

 private:
  unsigned int mCurrentFrameIndex;
  unsigned int mCurrentImageIndex;
  unsigned int mSwapchainImageCount;

  VkDescriptorSetLayout mDescriptorSetLayout;
  RenderPassRef mRenderPass;
  PipelineRef mPipeline;
  std::vector<std::unique_ptr<InFlightFrameResource>> mInFlightFrameResources;
  std::vector<std::unique_ptr<CommandBufferResources>> mCommandBufferResources;
  VkFormat mSurfaceFormat;

  SwapchainRef mSwapchain;
  const DeviceRef mDevice;

  // Initializers
  void initInFlightFrameResources();

  void initCommandResources();

  // Helpers

  void beginCommand(const VkCommandBuffer& commandBuffer);
  void setViewportConstrains(const VkCommandBuffer& commandBuffer,
                             const VkExtent2D extent);
  void beginRenderPass(const VkCommandBuffer& commandBuffer,
                       const FramebufferRef& framebuffer,
                       const RenderPassRef& renderPass,
                       const VkExtent2D extent,
                       const VkOffset2D offset = {0, 0},
                       const VkClearValue clearValue = {0, 0, 0, 0});
  void bindPipeline(const VkCommandBuffer& commandBuffer,
                    const PipelineRef& pipeline);
  void bindUniforms(const VkCommandBuffer& commandBuffer,
                    const UniformMatrixRef& uniformMatrix,
                    const PipelineRef& pipeline);
  void drawCommand(const VkCommandBuffer& commandBuffer);
  void endCommand(const VkCommandBuffer& commandBuffer);
  void present(const VkCommandBuffer& commandBuffer,
               const InFlightFrameResource& frameResources);
};

using RendererRef = std::shared_ptr<Renderer>;

}  // namespace Vulkan
}  // namespace Rendering
