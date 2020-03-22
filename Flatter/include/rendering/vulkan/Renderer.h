#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/Camera.h"
#include "rendering/vulkan/Context.h"
#include "rendering/vulkan/Framebuffer.h"
#include "rendering/vulkan/Pipeline.h"
#include "rendering/vulkan/RenderPass.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"
#include "rendering/vulkan/Uniform.h"

namespace Rendering {
namespace Vulkan {

using InFlightFrameResource = struct {
  vk::Fence frameFenceHandle;
  vk::Semaphore availableImageSemaphore;
  vk::Semaphore finishedRenderSemaphore;
};

using CommandBufferResources = struct {
  vk::CommandBuffer commandBuffer;
  FramebufferRef framebuffer;

  UniformMatrixRef matrixUniform;
};

const unsigned int FRAMES_IN_FLIGHT_COUNT = 2;

class Renderer {
 public:
  Renderer(const ContextRef& context, const SurfaceRef& surface);

  void draw(const Rendering::Camera& camera);

  virtual ~Renderer();

 private:
  unsigned int mCurrentFrameIndex;
  unsigned int mCurrentImageIndex;
  unsigned int mSwapchainImageCount;

  vk::DescriptorSetLayout mDescriptorSetLayout;
  RenderPassRef mRenderPass;
  PipelineRef mPipeline;
  std::vector<std::shared_ptr<InFlightFrameResource>> mInFlightFrameResources;
  std::vector<std::shared_ptr<CommandBufferResources>> mCommandBufferResources;
  vk::Format mSurfaceFormat;

  SwapchainRef mSwapchain;
  const ContextRef mContext;

  // Initializers
  void initInFlightFrameResources();

  void initCommandResources();

  // Helpers

  void beginCommand(const vk::CommandBuffer& commandBuffer);
  void setViewportConstrains(const vk::CommandBuffer& commandBuffer,
                             const vk::Extent2D extent);
  void beginRenderPass(const vk::CommandBuffer& commandBuffer,
                       const FramebufferRef& framebuffer,
                       const RenderPassRef& renderPass,
                       const vk::Extent2D extent,
                       const vk::Offset2D offset = vk::Offset2D(0, 0),
                       const vk::ClearValue clearValue = vk::ClearColorValue(
                           std::array<float, 4>({{0, 0, 0, 0}})));
  void bindPipeline(const vk::CommandBuffer& commandBuffer,
                    const PipelineRef& pipeline);
  void bindUniforms(const vk::CommandBuffer& commandBuffer,
                    const UniformMatrixRef& uniformMatrix,
                    const PipelineRef& pipeline);
  void draw(const vk::CommandBuffer& commandBuffer);
  void endCommand(const vk::CommandBuffer& commandBuffer);
  void present(const vk::CommandBuffer& commandBuffer,
               const InFlightFrameResource& frameResources);
};

using RendererRef = std::shared_ptr<Renderer>;

}  // namespace Vulkan
}  // namespace Rendering
