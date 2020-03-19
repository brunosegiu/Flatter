#include "rendering/vulkan/Renderer.h"

using namespace Rendering::Vulkan;

Renderer::Renderer(const DeviceRef& device,
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
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &uboLayoutBinding;

  vkCreateDescriptorSetLayout(mDevice->getHandle(), &layoutInfo, nullptr,
                              &mDescriptorSetLayout);

  // UNIFORM

  mPipeline =
      std::make_shared<Pipeline>(mDevice, mRenderPass, mDescriptorSetLayout);

  initCommandResources();
  initInFlightFrameResources();
}

void Rendering::Vulkan::Renderer::initInFlightFrameResources() {
  mInFlightFrameResources.reserve(FRAMES_IN_FLIGHT_COUNT);
  for (unsigned int frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT_COUNT;
       ++frameIndex) {
    std::unique_ptr<InFlightFrameResource> frameResources =
        std::make_unique<InFlightFrameResource>();

    const VkDevice& deviceHandle = mDevice->getHandle();
    const VkSemaphoreCreateInfo semaphoreCreateInfo = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkCreateSemaphore(deviceHandle, &semaphoreCreateInfo, 0,
                      &frameResources->availableImageSemaphore);
    vkCreateSemaphore(deviceHandle, &semaphoreCreateInfo, 0,
                      &frameResources->finishedRenderSemaphore);
    vkCreateFence(deviceHandle, &fenceCreateInfo, 0,
                  &frameResources->frameFenceHandle);

    frameResources->matrixUniform = std::make_shared<Uniform<glm::mat4>>(
        mDevice, mDescriptorSetLayout, glm::mat4(1.0f));

    mInFlightFrameResources.emplace_back(std::move(frameResources));
  }
}

void Rendering::Vulkan::Renderer::initCommandResources() {
  mCommandBufferResources.reserve(mSwapchainImageCount);
  for (unsigned int imageIndex = 0; imageIndex < mSwapchainImageCount;
       ++imageIndex) {
    std::unique_ptr<CommandBufferResources> commandResources =
        std::make_unique<CommandBufferResources>();

    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool = mDevice->getCommandPool();
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount = 1;
    const VkDevice& deviceHandle = mDevice->getHandle();
    vkAllocateCommandBuffers(deviceHandle, &commandBufferAllocInfo,
                             &commandResources->commandBuffer);
    commandResources->framebuffer = std::make_shared<Framebuffer>(
        mDevice, mSwapchain->getImage(imageIndex), mSurfaceFormat,
        mSwapchain->getExtent(), mRenderPass);
    mCommandBufferResources.emplace_back(std::move(commandResources));
  }
}

void Renderer::draw(const Camera& camera) {
  mCurrentFrameIndex = mCurrentFrameIndex++ % FRAMES_IN_FLIGHT_COUNT;

  const InFlightFrameResource& frameResources =
      *mInFlightFrameResources[mCurrentFrameIndex];

  const glm::mat4& mvp = camera.getViewProjection();
  frameResources.matrixUniform->update(mvp);
  mCurrentImageIndex = mSwapchain->acquireNextImage(
      frameResources.frameFenceHandle, frameResources.availableImageSemaphore);
  assert(mCurrentImageIndex < mSwapchainImageCount);
  const CommandBufferResources& commandResources =
      *mCommandBufferResources[mCurrentImageIndex];
  const VkCommandBuffer& currentCommandBuffer = commandResources.commandBuffer;
  const VkExtent2D imageExtent = mSwapchain->getExtent();

  beginCommand(currentCommandBuffer);
  bindPipeline(currentCommandBuffer, mPipeline);
  setViewportConstrains(currentCommandBuffer, imageExtent);
  beginRenderPass(currentCommandBuffer, commandResources.framebuffer,
                  mRenderPass, imageExtent);
  bindUniforms(currentCommandBuffer, frameResources.matrixUniform, mPipeline);
  drawCommand(currentCommandBuffer);
  endCommand(currentCommandBuffer);
  present(currentCommandBuffer, frameResources);
}

void Renderer::beginCommand(const VkCommandBuffer& commandBuffer) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void Renderer::beginRenderPass(const VkCommandBuffer& commandBuffer,
                               const FramebufferRef& framebuffer,
                               const RenderPassRef& renderPass,
                               const VkExtent2D extent,
                               const VkOffset2D offset,
                               const VkClearValue clearValue) {
  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = renderPass->getHandle();
  renderPassBeginInfo.framebuffer = framebuffer->getHandle();
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearValue;
  renderPassBeginInfo.renderArea.offset = offset;
  renderPassBeginInfo.renderArea.extent = extent;
  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::bindPipeline(const VkCommandBuffer& commandBuffer,
                            const PipelineRef& pipeline) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline->getHandle());
}

void Renderer::setViewportConstrains(const VkCommandBuffer& commandBuffer,
                                     const VkExtent2D extent) {
  VkViewport viewport{0.0f,
                      0.0f,
                      static_cast<float>(extent.width),
                      static_cast<float>(extent.height),
                      0.0f,
                      1.0f};

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  VkRect2D scissors{{0, 0}, extent};
  vkCmdSetScissor(commandBuffer, 0, 1, &scissors);
}

void Renderer::bindUniforms(const VkCommandBuffer& commandBuffer,
                            const UniformMatrixRef& uniformMatrix,
                            const PipelineRef& pipeline) {
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline->getPipelineLayoutHandle(), 0, 1,
                          &uniformMatrix->getDescriptorHandle(), 0, nullptr);
}

void Renderer::drawCommand(const VkCommandBuffer& commandBuffer) {
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void Renderer::endCommand(const VkCommandBuffer& commandBuffer) {
  vkCmdEndRenderPass(commandBuffer);
  vkEndCommandBuffer(commandBuffer);
}

void Renderer::present(const VkCommandBuffer& commandBuffer,
                       const InFlightFrameResource& frameResources) {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &frameResources.availableImageSemaphore;
  VkPipelineStageFlags pipelineStageFlags{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.pWaitDstStageMask = &pipelineStageFlags;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &frameResources.finishedRenderSemaphore;

  const VkQueue& queueHandle = mDevice->getQueueHandle();
  vkQueueSubmit(queueHandle, 1, &submitInfo, frameResources.frameFenceHandle);

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &frameResources.finishedRenderSemaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &mSwapchain->getHandle();
  presentInfo.pImageIndices = &mCurrentImageIndex;

  vkQueuePresentKHR(queueHandle, &presentInfo);
}

Renderer::~Renderer() {}
