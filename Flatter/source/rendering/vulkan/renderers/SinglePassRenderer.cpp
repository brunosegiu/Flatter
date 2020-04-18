#include "rendering/vulkan/renderers/SinglePassRenderer.h"

using namespace Rendering::Vulkan;

SinglePassRenderer::SinglePassRenderer(const ContextRef& context,
                                       const SurfaceRef& surface)
    : Renderer(context) {
  mDepthBuffer = std::make_shared<DepthBufferAttachment>(
      mContext, mContext->getSwapchain()->getExtent());
  mRenderPass = std::make_shared<SingleRenderPass>(
      context, surface->getFormat(context->getPhysicalDevice()).format,
      mDepthBuffer->getFormat());
  const vk::Device& device = mContext->getDevice();
  // UNIFORM
  auto const uboLayoutBinding =
      vk::DescriptorSetLayoutBinding{}
          .setBinding(0)
          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
          .setDescriptorCount(1)
          .setStageFlags(vk::ShaderStageFlagBits::eVertex);

  auto const layoutInfo =
      vk::DescriptorSetLayoutCreateInfo{}.setBindingCount(1).setPBindings(
          &uboLayoutBinding);

  device.createDescriptorSetLayout(&layoutInfo, nullptr, &mDescriptorSetLayout);

  // UNIFORM

  mPipeline = std::make_shared<SinglePassPipeline>(mContext, mRenderPass,
                                                   mDescriptorSetLayout);

  mScreenFramebufferRing = std::make_shared<ScreenFramebufferRing>(
      mContext, surface, mRenderPass, mDescriptorSetLayout, mDepthBuffer);
}

void SinglePassRenderer::draw(Camera& camera, const SceneRef& scene) {
  const glm::mat4& mvp = camera.getViewProjection();
  const RenderingResources& resources = mScreenFramebufferRing->swapBuffers();
  resources.matrixUniform->update(mvp);
  const vk::CommandBuffer& currentCommandBuffer(resources.commandBuffer);

  const vk::Extent2D& imageExtent = mContext->getSwapchain()->getExtent();
  beginCommand(currentCommandBuffer);
  bindPipeline(currentCommandBuffer, mPipeline);
  setViewportConstrains(currentCommandBuffer, imageExtent);
  beginRenderPass(currentCommandBuffer, resources.framebuffer, mRenderPass,
                  imageExtent);
  bindUniforms(currentCommandBuffer, resources.matrixUniform, mPipeline);

  for (const auto& mesh : scene->getMeshes()) {
    const IndexedVertexBufferRef& indexedVertexBuffer =
        mesh->getIndexedVertexBuffer();
    vk::DeviceSize offset{0};
    currentCommandBuffer.bindVertexBuffers(
        0, 1, &indexedVertexBuffer->getVertexBuffer(), &offset);
    currentCommandBuffer.bindIndexBuffer(indexedVertexBuffer->getIndexBuffer(),
                                         offset, vk::IndexType::eUint16);
    currentCommandBuffer.drawIndexed(indexedVertexBuffer->getIndexCount(), 1, 0,
                                     0, 0);
  }

  endCommand(currentCommandBuffer);
  present(currentCommandBuffer, resources.imageAvailableSemaphore,
          resources.imageRenderedSemaphore, resources.frameInUseFence,
          resources.imageIndex);
}

void SinglePassRenderer::beginCommand(const vk::CommandBuffer& commandBuffer) {
  auto const beginInfo = vk::CommandBufferBeginInfo{}.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  commandBuffer.begin(&beginInfo);
}

void SinglePassRenderer::beginRenderPass(const vk::CommandBuffer& commandBuffer,
                                         const FramebufferRef& framebuffer,
                                         const RenderPassRef& renderPass,
                                         const vk::Extent2D extent,
                                         const vk::Offset2D offset,
                                         const vk::ClearValue clearValue) {
  vk::Rect2D renderArea(offset, extent);

  std::array<vk::ClearValue, 2> clearValues{};
  clearValues[0].setColor(
      vk::ClearColorValue(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f}));
  clearValues[1].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 1u));

  auto const renderPassBeginInfo =
      vk::RenderPassBeginInfo{}
          .setRenderPass(renderPass->getHandle())
          .setFramebuffer(framebuffer->getHandle())
          .setClearValueCount(static_cast<unsigned int>(clearValues.size()))
          .setPClearValues(clearValues.data())
          .setRenderArea(renderArea);
  commandBuffer.beginRenderPass(&renderPassBeginInfo,
                                vk::SubpassContents::eInline);
}

void SinglePassRenderer::bindPipeline(const vk::CommandBuffer& commandBuffer,
                                      const SinglePassPipelineRef& pipeline) {
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             pipeline->getHandle());
}

void SinglePassRenderer::setViewportConstrains(
    const vk::CommandBuffer& commandBuffer,
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

void SinglePassRenderer::bindUniforms(const vk::CommandBuffer& commandBuffer,
                                      const UniformMatrixRef& uniformMatrix,
                                      const SinglePassPipelineRef& pipeline) {
  commandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayout(), 0, 1,
      &uniformMatrix->getDescriptorHandle(), 0, nullptr);
}

void SinglePassRenderer::draw(const vk::CommandBuffer& commandBuffer) {
  commandBuffer.draw(3, 1, 0, 0);
}

void SinglePassRenderer::endCommand(const vk::CommandBuffer& commandBuffer) {
  commandBuffer.endRenderPass();
  commandBuffer.end();
}

void SinglePassRenderer::present(const vk::CommandBuffer& commandBuffer,
                                 const vk::Semaphore& imageAvailableSemaphore,
                                 const vk::Semaphore& renderingDoneSemaphore,
                                 const vk::Fence& presentFrameFence,
                                 const unsigned int imageIndex) {
  const SwapchainRef& swapchain = mContext->getSwapchain();

  const vk::PipelineStageFlags pipelineStageFlags{
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  auto const submitInfo = vk::SubmitInfo{}
                              .setWaitSemaphoreCount(1)
                              .setPWaitSemaphores(&imageAvailableSemaphore)
                              .setPWaitDstStageMask(&pipelineStageFlags)
                              .setCommandBufferCount(1)
                              .setPCommandBuffers(&commandBuffer)
                              .setSignalSemaphoreCount(1)
                              .setPSignalSemaphores(&renderingDoneSemaphore);
  const vk::Queue& queue = mContext->getQueue();
  const vk::Result submitResult =
      queue.submit(1, &submitInfo, presentFrameFence);
  assert(submitResult == vk::Result::eSuccess);
  auto const presentInfo = vk::PresentInfoKHR{}
                               .setWaitSemaphoreCount(1)
                               .setPWaitSemaphores(&renderingDoneSemaphore)
                               .setSwapchainCount(1)
                               .setPSwapchains(&swapchain->getHandle())
                               .setPImageIndices(&imageIndex);
  queue.presentKHR(&presentInfo);
}

SinglePassRenderer::~SinglePassRenderer() {}
