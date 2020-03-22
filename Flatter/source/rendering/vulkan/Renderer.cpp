#include "rendering/vulkan/Renderer.h"

using namespace Rendering::Vulkan;

Renderer::Renderer(const SingleDeviceRef& device,
                   const SurfaceRef& surface,
                   const SwapchainRef& swapchain)
    : mDevice(device),
      mSwapchain(swapchain),
      mCurrentFrameIndex(0),
      mCurrentImageIndex(0),
      mSwapchainImageCount(swapchain->getImageCount()),
      mSurfaceFormat(surface->getSurfaceFormat(device).format) {
  mRenderPass = std::make_shared<RenderPass>(device, mSurfaceFormat);

  // UNIFORM
  auto const uboLayoutBinding =
      vk::DescriptorSetLayoutBinding()
          .setBinding(0)
          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
          .setDescriptorCount(1)
          .setStageFlags(vk::ShaderStageFlagBits::eVertex);

  auto const layoutInfo =
      vk::DescriptorSetLayoutCreateInfo().setBindingCount(1).setPBindings(
          &uboLayoutBinding);

  mDevice->createDescriptorSetLayout(&layoutInfo, nullptr,
                                     &mDescriptorSetLayout);

  // UNIFORM

  mPipeline =
      std::make_shared<Pipeline>(mDevice, mRenderPass, mDescriptorSetLayout);

  initCommandResources();
  initInFlightFrameResources();
}

void Renderer::initInFlightFrameResources() {
  mInFlightFrameResources.reserve(FRAMES_IN_FLIGHT_COUNT);
  for (unsigned int frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT_COUNT;
       ++frameIndex) {
    std::shared_ptr<InFlightFrameResource> frameResources =
        std::make_shared<InFlightFrameResource>();

    const vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    auto const fenceCreateInfo =
        vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

    mDevice->createSemaphore(&semaphoreCreateInfo, nullptr,
                             &frameResources->availableImageSemaphore);
    mDevice->createSemaphore(&semaphoreCreateInfo, nullptr,
                             &frameResources->finishedRenderSemaphore);
    mDevice->createFence(&fenceCreateInfo, nullptr,
                         &frameResources->frameFenceHandle);

    frameResources->matrixUniform = std::make_shared<Uniform<glm::mat4>>(
        mDevice, mDescriptorSetLayout, glm::mat4(1.0f));

    mInFlightFrameResources.push_back(std::move(frameResources));
  }
}

void Renderer::initCommandResources() {
  mCommandBufferResources.reserve(mSwapchainImageCount);
  for (unsigned int imageIndex = 0; imageIndex < mSwapchainImageCount;
       ++imageIndex) {
    std::shared_ptr<CommandBufferResources> commandResources =
        std::make_shared<CommandBufferResources>();

    auto const commandBufferAllocInfo =
        vk::CommandBufferAllocateInfo()
            .setCommandPool(mDevice->getCommandPool())
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
    mDevice->allocateCommandBuffers(&commandBufferAllocInfo,
                                    &commandResources->commandBuffer);
    commandResources->framebuffer = std::make_shared<Framebuffer>(
        mDevice, mSwapchain->getImage(imageIndex), mSurfaceFormat,
        mSwapchain->getExtent(), mRenderPass);
    mCommandBufferResources.push_back(std::move(commandResources));
  }
}

void Renderer::draw(const Camera& camera) {
  mCurrentFrameIndex = (mCurrentFrameIndex++) % FRAMES_IN_FLIGHT_COUNT;

  const InFlightFrameResource& frameResources =
      *mInFlightFrameResources[mCurrentFrameIndex];

  const glm::mat4& mvp = camera.getViewProjection();
  frameResources.matrixUniform->update(mvp);
  mCurrentImageIndex = mSwapchain->acquireNextImage(
      frameResources.frameFenceHandle, frameResources.availableImageSemaphore);
  assert(mCurrentImageIndex < mSwapchainImageCount);
  const CommandBufferResources& commandResources =
      *mCommandBufferResources[mCurrentImageIndex];
  const vk::CommandBuffer& currentCommandBuffer =
      commandResources.commandBuffer;
  const vk::Extent2D& imageExtent = mSwapchain->getExtent();

  beginCommand(currentCommandBuffer);
  bindPipeline(currentCommandBuffer, mPipeline);
  setViewportConstrains(currentCommandBuffer, imageExtent);
  beginRenderPass(currentCommandBuffer, commandResources.framebuffer,
                  mRenderPass, imageExtent);
  bindUniforms(currentCommandBuffer, frameResources.matrixUniform, mPipeline);
  draw(currentCommandBuffer);
  endCommand(currentCommandBuffer);
  present(currentCommandBuffer, frameResources);
}

void Renderer::beginCommand(const vk::CommandBuffer& commandBuffer) {
  auto const beginInfo = vk::CommandBufferBeginInfo().setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  commandBuffer.begin(&beginInfo);
}

void Renderer::beginRenderPass(const vk::CommandBuffer& commandBuffer,
                               const FramebufferRef& framebuffer,
                               const RenderPassRef& renderPass,
                               const vk::Extent2D extent,
                               const vk::Offset2D offset,
                               const vk::ClearValue clearValue) {
  vk::Rect2D renderArea(offset, extent);
  auto const renderPassBeginInfo = vk::RenderPassBeginInfo()
                                       .setRenderPass(renderPass->getHandle())
                                       .setFramebuffer(framebuffer->getHandle())
                                       .setClearValueCount(1)
                                       .setPClearValues(&clearValue)
                                       .setRenderArea(renderArea);
  commandBuffer.beginRenderPass(&renderPassBeginInfo,
                                vk::SubpassContents::eInline);
}

void Renderer::bindPipeline(const vk::CommandBuffer& commandBuffer,
                            const PipelineRef& pipeline) {
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             pipeline->getHandle());
}

void Renderer::setViewportConstrains(const vk::CommandBuffer& commandBuffer,
                                     const vk::Extent2D extent) {
  const vk::Viewport viewport{0.0f,
                              0.0f,
                              static_cast<float>(extent.width),
                              static_cast<float>(extent.height),
                              0.0f,
                              1.0f};

  commandBuffer.setViewport(0, 1, &viewport);
  vk::Rect2D scissors{{0, 0}, extent};
  commandBuffer.setScissor(0, 1, &scissors);
}

void Renderer::bindUniforms(const vk::CommandBuffer& commandBuffer,
                            const UniformMatrixRef& uniformMatrix,
                            const PipelineRef& pipeline) {
  commandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayoutHandle(), 0,
      1, &uniformMatrix->getDescriptorHandle(), 0, nullptr);
}

void Renderer::draw(const vk::CommandBuffer& commandBuffer) {
  commandBuffer.draw(3, 1, 0, 0);
}

void Renderer::endCommand(const vk::CommandBuffer& commandBuffer) {
  commandBuffer.endRenderPass();
  commandBuffer.end();
}

void Renderer::present(const vk::CommandBuffer& commandBuffer,
                       const InFlightFrameResource& frameResources) {
  const vk::PipelineStageFlags pipelineStageFlags{
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  auto const submitInfo =
      vk::SubmitInfo()
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(&frameResources.availableImageSemaphore)
          .setPWaitDstStageMask(&pipelineStageFlags)
          .setCommandBufferCount(1)
          .setPCommandBuffers(&commandBuffer)
          .setSignalSemaphoreCount(1)
          .setPSignalSemaphores(&frameResources.finishedRenderSemaphore);
  const vk::Queue& queueHandle = mDevice->getQueueHandle();
  queueHandle.submit(1, &submitInfo, frameResources.frameFenceHandle);
  auto const presentInfo =
      vk::PresentInfoKHR()
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(&frameResources.finishedRenderSemaphore)
          .setSwapchainCount(1)
          .setPSwapchains(&mSwapchain->getHandle())
          .setPImageIndices(&mCurrentImageIndex);
  queueHandle.presentKHR(&presentInfo);
}

Renderer::~Renderer() {}
