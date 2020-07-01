#include "rendering//ScreenFramebufferRing.h"

using namespace Rendering::Vulkan;

ScreenFramebufferRing::ScreenFramebufferRing(
    const ContextRef& context,
    const SurfaceRef& surface,
    const RenderPassRef& renderPass,
    const DepthBufferAttachmentRef& depthBufferAtt)
    : mContext(context),
      mSurface(surface),
      mSurfaceFormat(surface->getFormat(context->getPhysicalDevice()).format),
      mImageCount(context->getSwapchain()->getImageCount()),
      mCurrentFrameIndex(0) {
  initImagesResources(renderPass, depthBufferAtt);
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
        vk::FenceCreateInfo{}.setFlags(vk::FenceCreateFlagBits::eSignaled);

    device.createSemaphore(&semaphoreCreateInfo, nullptr,
                           &frameResources.imageAvailableSemaphore);
    device.createSemaphore(&semaphoreCreateInfo, nullptr,
                           &frameResources.imageRenderedSemaphore);
    device.createFence(&fenceCreateInfo, nullptr,
                       &frameResources.frameInUseFence);
  }
}

void ScreenFramebufferRing::initImagesResources(
    const RenderPassRef& renderPass,
    const DepthBufferAttachmentRef& depthBufferAtt) {
  mImagesResources.reserve(mImageCount);
  const vk::Device& device = mContext->getDevice();
  const SwapchainRef& swapchain = mContext->getSwapchain();
  for (unsigned int imageIndex = 0; imageIndex < mImageCount; ++imageIndex) {
    mImagesResources.emplace_back();
    SwapchainImageResources& commandResources = mImagesResources[imageIndex];

    commandResources.framebuffer = std::make_shared<SwapchainFramebuffer>(
        mContext, swapchain->getImage(imageIndex), mSurfaceFormat,
        swapchain->getExtent(), renderPass, depthBufferAtt);

    auto const commandBufferAllocInfo =
        vk::CommandBufferAllocateInfo{}
            .setCommandPool(mContext->getCommandPool())
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
    device.allocateCommandBuffers(&commandBufferAllocInfo,
                                  &commandResources.commandBuffer);
  }
}

const RenderingResources ScreenFramebufferRing::swapBuffers() {
  const InFlightFrameResources& frameResources(
      mInFlightFrameResources[mCurrentFrameIndex]);
  const SwapchainRef& swapchain = mContext->getSwapchain();
  mCurrentImageIndex = swapchain->acquireNextImage(
      frameResources.frameInUseFence, frameResources.imageAvailableSemaphore);
  assert(mCurrentImageIndex < mImageCount);
  const SwapchainImageResources& commandResources(
      mImagesResources[mCurrentImageIndex]);
  mCurrentFrameIndex = (++mCurrentFrameIndex) % FRAMES_IN_FLIGHT_COUNT;
  return RenderingResources(
      frameResources.frameInUseFence, frameResources.imageAvailableSemaphore,
      frameResources.imageRenderedSemaphore, commandResources.commandBuffer,
      commandResources.framebuffer, mCurrentImageIndex);
}

ScreenFramebufferRing::~ScreenFramebufferRing() {}