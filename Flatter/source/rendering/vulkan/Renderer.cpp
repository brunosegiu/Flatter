#include "rendering/vulkan/Renderer.h"

using namespace Rendering::Vulkan;

Renderer::Renderer(const ContextRef& context, const SurfaceRef& surface)
    : mContext(context), mVertices(context) {
  mRenderPass = std::make_shared<RenderPass>(
      context, surface->getFormat(context->getPhysicalDevice()).format);
  const vk::Device& device = mContext->getDevice();
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

  device.createDescriptorSetLayout(&layoutInfo, nullptr, &mDescriptorSetLayout);

  // UNIFORM

  mPipeline = std::make_shared<Pipeline>(
      mContext, mRenderPass, mDescriptorSetLayout, mVertices.mDescription,
      mVertices.mAttrDescription);

  mScreenFramebufferRing = std::make_shared<ScreenFramebufferRing>(
      mContext, surface, mRenderPass, mDescriptorSetLayout);
}

void Renderer::draw(const Camera& camera) {
  const glm::mat4& mvp = camera.getViewProjection();
  const RenderingResources& resources = mScreenFramebufferRing->cycle();
  resources.matrixUniform->update(mvp);
  const vk::CommandBuffer& currentCommandBuffer(resources.commandBuffer);
  const vk::Extent2D& imageExtent = mContext->getSwapchain()->getExtent();
  beginCommand(currentCommandBuffer);
  bindPipeline(currentCommandBuffer, mPipeline);
  setViewportConstrains(currentCommandBuffer, imageExtent);
  beginRenderPass(currentCommandBuffer, resources.framebuffer, mRenderPass,
                  imageExtent);
  bindUniforms(currentCommandBuffer, resources.matrixUniform, mPipeline);

  vk::DeviceSize offset{0};
  currentCommandBuffer.bindVertexBuffers(0, 1, &mVertices.mBuffer, &offset);
  currentCommandBuffer.draw(static_cast<uint32_t>(mVertices.mVertices.size()),
                            1, 0, 0);
  endCommand(currentCommandBuffer);
  present(currentCommandBuffer, resources.availableImageSemaphore,
          resources.finishedRenderSemaphore, resources.frameFenceHandle,
          resources.imageIndex);
}

void Renderer::beginCommand(const vk::CommandBuffer& commandBuffer) {
  auto const beginInfo = vk::CommandBufferBeginInfo{}.setFlags(
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
  auto const renderPassBeginInfo = vk::RenderPassBeginInfo{}
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
                       const vk::Semaphore& imageAvailableSemaphore,
                       const vk::Semaphore& renderingDoneSemaphore,
                       const vk::Fence& presentFrameFence,
                       const unsigned int imageIndex) {
  const SwapchainRef& swapchain = mContext->getSwapchain();

  const vk::PipelineStageFlags pipelineStageFlags{
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  auto const submitInfo = vk::SubmitInfo()
                              .setWaitSemaphoreCount(1)
                              .setPWaitSemaphores(&imageAvailableSemaphore)
                              .setPWaitDstStageMask(&pipelineStageFlags)
                              .setCommandBufferCount(1)
                              .setPCommandBuffers(&commandBuffer)
                              .setSignalSemaphoreCount(1)
                              .setPSignalSemaphores(&renderingDoneSemaphore);
  const vk::Queue& queue = mContext->getQueue();
  queue.submit(1, &submitInfo, presentFrameFence);
  auto const presentInfo = vk::PresentInfoKHR()
                               .setWaitSemaphoreCount(1)
                               .setPWaitSemaphores(&renderingDoneSemaphore)
                               .setSwapchainCount(1)
                               .setPSwapchains(&swapchain->getHandle())
                               .setPImageIndices(&imageIndex);
  queue.presentKHR(&presentInfo);
}

Renderer::~Renderer() {}
