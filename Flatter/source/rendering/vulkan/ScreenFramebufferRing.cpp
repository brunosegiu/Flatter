#include "rendering/vulkan/ScreenFramebufferRing.h"

using namespace Rendering::Vulkan;

ScreenFramebufferRing::ScreenFramebufferRing(
    const ContextRef& context,
    const SurfaceRef& surface,
    const RenderPassRef& renderPass,
    const VkDescriptorSetLayout& descriptorSetLayout)
    : mContext(context),
      mSurface(surface),
      mSurfaceFormat(surface->getFormat(context->getPhysicalDevice()).format),
      mSwapchainImageCount(context->getSwapchain()->getImageCount()) {
  initCommandResources(renderPass, descriptorSetLayout);
  initInFlightFrameResources();
}

void ScreenFramebufferRing::initInFlightFrameResources() {
  mInFlightFrameResources.reserve(FRAMES_IN_FLIGHT_COUNT);
  const vk::Device& device = mContext->getDevice();
  for (unsigned int frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT_COUNT;
       ++frameIndex) {
    mInFlightFrameResources.emplace_back();
    InFlightFrameResources& frameResources =
        mInFlightFrameResources[frameIndex];
    const vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    auto const fenceCreateInfo =
        vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

    device.createSemaphore(&semaphoreCreateInfo, nullptr,
                           &frameResources.availableImageSemaphore);
    device.createSemaphore(&semaphoreCreateInfo, nullptr,
                           &frameResources.finishedRenderSemaphore);
    device.createFence(&fenceCreateInfo, nullptr,
                       &frameResources.frameFenceHandle);
  }
}

void ScreenFramebufferRing::initCommandResources(
    const RenderPassRef& renderPass,
    const VkDescriptorSetLayout& descriptorSetLayout) {
  mCommandBufferResources.reserve(mSwapchainImageCount);
  const vk::Device& device = mContext->getDevice();
  const SwapchainRef& swapchain = mContext->getSwapchain();
  for (unsigned int imageIndex = 0; imageIndex < mSwapchainImageCount;
       ++imageIndex) {
    mCommandBufferResources.emplace_back();
    CommandBufferResources& commandResources =
        mCommandBufferResources[imageIndex];
    auto const commandBufferAllocInfo =
        vk::CommandBufferAllocateInfo()
            .setCommandPool(mContext->getCommandPool())
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
    device.allocateCommandBuffers(&commandBufferAllocInfo,
                                  &commandResources.commandBuffer);
    commandResources.framebuffer = std::make_shared<Framebuffer>(
        mContext, swapchain->getImage(imageIndex), mSurfaceFormat,
        swapchain->getExtent(), renderPass);
    commandResources.matrixUniform = std::make_shared<Uniform<glm::mat4>>(
        mContext, descriptorSetLayout, glm::mat4(1.0f));
  }
}

const RenderingResources ScreenFramebufferRing::cycle() {
  mCurrentFrameIndex = (mCurrentFrameIndex++) % FRAMES_IN_FLIGHT_COUNT;
  const InFlightFrameResources& frameResources(
      mInFlightFrameResources[mCurrentFrameIndex]);
  const SwapchainRef& swapchain = mContext->getSwapchain();
  mCurrentImageIndex = swapchain->acquireNextImage(
      frameResources.frameFenceHandle, frameResources.availableImageSemaphore);
  assert(mCurrentImageIndex < mSwapchainImageCount);
  const CommandBufferResources& commandResources(
      mCommandBufferResources[mCurrentImageIndex]);

  return RenderingResources(
      frameResources.frameFenceHandle, frameResources.availableImageSemaphore,
      frameResources.finishedRenderSemaphore, commandResources.commandBuffer,
      commandResources.framebuffer, commandResources.matrixUniform,
      mCurrentImageIndex);
}

ScreenFramebufferRing::~ScreenFramebufferRing() {}
