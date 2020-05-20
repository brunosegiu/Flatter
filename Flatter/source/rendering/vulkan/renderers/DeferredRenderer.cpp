#include "rendering/vulkan/renderers/DeferredRenderer.h"

using namespace Rendering::Vulkan;

DeferredRenderer::DeferredRenderer(const ContextRef& context,
                                   const SurfaceRef& surface)
    : Renderer(context) {
  // Shared resources
  const vk::Device& device = mContext->getDevice();
  const vk::Extent2D extent(mContext->getSwapchain()->getExtent());
  mDescriptorPool = std::make_shared<DescriptorPool>(mContext, 1, 3, 10);

  // GBuffer resources
  mDeferredLayout = std::make_shared<DescriptorLayout>(
      mContext,
      std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>>{
          {vk::DescriptorType::eUniformBuffer,
           vk::ShaderStageFlagBits::eVertex},
      });
  mColorAtt = std::make_shared<FramebufferAttachment>(
      mContext, extent, vk::Format::eR8G8B8A8Unorm,
      vk::ImageUsageFlagBits::eColorAttachment |
          vk::ImageUsageFlagBits::eSampled,
      vk::ImageAspectFlagBits::eColor);
  mPosAtt = std::make_shared<FramebufferAttachment>(
      mContext, extent, vk::Format::eR16G16B16A16Sfloat,
      vk::ImageUsageFlagBits::eColorAttachment |
          vk::ImageUsageFlagBits::eSampled,
      vk::ImageAspectFlagBits::eColor);
  mNormAtt = std::make_shared<FramebufferAttachment>(
      mContext, extent, vk::Format::eR16G16B16A16Sfloat,
      vk::ImageUsageFlagBits::eColorAttachment |
          vk::ImageUsageFlagBits::eSampled,
      vk::ImageAspectFlagBits::eColor);
  mDepthAtt = std::make_shared<DepthBufferAttachment>(mContext, extent);

  mDeferredPipeline = std::make_shared<DeferredPipeline>(
      mContext, mDeferredLayout, mColorAtt->getFormat(), mPosAtt->getFormat(),
      mNormAtt->getFormat(), mDepthAtt->getFormat());

  mGBuffer = std::make_shared<Framebuffer>(
      mContext, mDeferredPipeline,
      std::vector<FramebufferAttachmentRef>{mColorAtt, mPosAtt, mNormAtt,
                                            mDepthAtt},
      extent);

  mMatrixUniform = std::make_shared<Uniform<glm::mat4>>(
      mContext, mDeferredLayout, mDescriptorPool,
      static_cast<unsigned int>(
          DeferredPipeline::Descriptors::ModelViewProjectionMatrix),
      glm::mat4(1.0f));

  // Fullscreen pass resources
  mFullscreenLayout = std::make_shared<DescriptorLayout>(
      mContext,
      std::vector<std::pair<vk::DescriptorType, vk::ShaderStageFlags>>{
          {vk::DescriptorType::eCombinedImageSampler,
           vk::ShaderStageFlagBits::eFragment},
          {vk::DescriptorType::eCombinedImageSampler,
           vk::ShaderStageFlagBits::eFragment},
          {vk::DescriptorType::eCombinedImageSampler,
           vk::ShaderStageFlagBits::eFragment}});
  mFullscreenRenderPass = std::make_shared<FullscreenRenderPass>(
      mContext, surface->getFormat(mContext->getPhysicalDevice()).format);
  mFullscreenPipeline = std::make_shared<FullscreenPipeline>(
      mContext, mFullscreenLayout, mFullscreenRenderPass);

  mScreenFramebufferRing = std::make_shared<ScreenFramebufferRing>(
      mContext, surface, mFullscreenRenderPass);

  auto const descriptorAllocInfo =
      vk::DescriptorSetAllocateInfo{}
          .setDescriptorPool(mDescriptorPool->getDescriptorPool())
          .setDescriptorSetCount(1)
          .setPSetLayouts(&mFullscreenLayout->getHandle());
  assert(device.allocateDescriptorSets(&descriptorAllocInfo, &mDescriptorSet) ==
         vk::Result::eSuccess);

  mAlbedoSampler =
      std::make_shared<Sampler>(mContext, mDescriptorSet, mColorAtt, 0);
  mPositionSampler =
      std::make_shared<Sampler>(mContext, mDescriptorSet, mPosAtt, 1);
  mNormalSampler =
      std::make_shared<Sampler>(mContext, mDescriptorSet, mNormAtt, 2);

  auto const commandBufferAllocInfo =
      vk::CommandBufferAllocateInfo{}
          .setCommandPool(mContext->getCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandBufferCount(1);
  mContext->getDevice().allocateCommandBuffers(&commandBufferAllocInfo,
                                               &mDeferredCB);

  const auto offscreenSemaphoreCreateInfo = vk::SemaphoreCreateInfo{};
  const auto semaphoreRes =
      mContext->getDevice().createSemaphore(offscreenSemaphoreCreateInfo);
  assert(semaphoreRes.result == vk::Result::eSuccess);
  mOffscreenSemaphore = semaphoreRes.value;

  auto const fenceCreateInfo =
      vk::FenceCreateInfo{}.setFlags(vk::FenceCreateFlagBits::eSignaled);
  device.createFence(&fenceCreateInfo, nullptr, &mDeferredFence);
}

void DeferredRenderer::draw(Rendering::Camera& camera, const SceneRef& scene) {
  const RenderingResources& resources = mScreenFramebufferRing->swapBuffers();
  drawDeferred(resources, camera, scene);
  drawFullscreen(resources);
}

void DeferredRenderer::drawDeferred(const RenderingResources& resources,
                                    Rendering::Camera& camera,
                                    const SceneRef& scene) {
  const vk::Result waitResult = mContext->getDevice().waitForFences(
      1, &mDeferredFence, VK_TRUE, UINT64_MAX);
  assert(waitResult == vk::Result::eSuccess);
  assert(mContext->getDevice().resetFences(1, &mDeferredFence) ==
         vk::Result::eSuccess);
  const glm::mat4& mvp = camera.getViewProjection();
  mMatrixUniform->update(mvp);
  const auto cmdBeginInfo = vk::CommandBufferBeginInfo{};
  std::array<vk::ClearValue, 4> clearValues{};
  clearValues[0].setColor(
      vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
  clearValues[1].setColor(
      vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
  clearValues[2].setColor(
      vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
  clearValues[3].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 1u));
  const vk::Extent2D screenExtent = mContext->getSwapchain()->getExtent();
  const vk::Rect2D extent{{0, 0}, screenExtent};
  const auto renderPassBeginInfo =
      vk::RenderPassBeginInfo{}
          .setRenderPass(mDeferredPipeline->getRenderPass()->getHandle())
          .setFramebuffer(mGBuffer->getHandle())
          .setRenderArea(extent)
          .setClearValueCount(static_cast<unsigned int>(clearValues.size()))
          .setPClearValues(clearValues.data());

  mDeferredCB.begin(cmdBeginInfo);
  mDeferredCB.beginRenderPass(renderPassBeginInfo,
                              vk::SubpassContents::eInline);
  const vk::Viewport viewport{0.0f,
                              0.0f,
                              static_cast<float>(screenExtent.width),
                              static_cast<float>(screenExtent.height),
                              0.0f,
                              1.0f};
  mDeferredCB.setViewport(0, 1, &viewport);
  mDeferredCB.setScissor(0, 1, &extent);
  mDeferredCB.bindPipeline(vk::PipelineBindPoint::eGraphics,
                           mDeferredPipeline->getHandle());
  mDeferredCB.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, mDeferredPipeline->getPipelineLayout(),
      0, 1, &mMatrixUniform->getDescriptorHandle(), 0, nullptr);
  for (const auto& mesh : scene->getMeshes()) {
    const IndexedVertexBufferRef& indexedVertexBuffer =
        mesh->getIndexedVertexBuffer();
    vk::DeviceSize offset{0};
    mDeferredCB.bindVertexBuffers(0, 1, &indexedVertexBuffer->getVertexBuffer(),
                                  &offset);
    mDeferredCB.bindIndexBuffer(indexedVertexBuffer->getIndexBuffer(), offset,
                                IndexedVertexBuffer::sIndexType);
    mDeferredCB.drawIndexed(indexedVertexBuffer->getIndexCount(), 1, 0, 0, 0);
  }
  mDeferredCB.endRenderPass();
  mDeferredCB.end();
  submit(mDeferredCB, resources.imageAvailableSemaphore, mOffscreenSemaphore,
         mDeferredFence);
}

void DeferredRenderer::drawFullscreen(const RenderingResources& resources) {
  const vk::CommandBuffer& currentCommandBuffer(resources.commandBuffer);
  auto const beginInfo = vk::CommandBufferBeginInfo{}.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  currentCommandBuffer.begin(&beginInfo);
  currentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                    mFullscreenPipeline->getHandle());
  currentCommandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      mFullscreenPipeline->getPipelineLayout(), 0, 1, &mDescriptorSet, 0,
      nullptr);
  const auto clearValues = vk::ClearValue{}.setColor(
      vk::ClearColorValue(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f}));
  const vk::Extent2D screenExtent = mContext->getSwapchain()->getExtent();
  const vk::Rect2D extent{{0, 0}, screenExtent};

  const auto renderPassBeginInfo =
      vk::RenderPassBeginInfo{}
          .setRenderPass(mFullscreenRenderPass->getHandle())
          .setFramebuffer(resources.framebuffer->getHandle())
          .setRenderArea(extent)
          .setClearValueCount(1)
          .setPClearValues(&clearValues);

  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);
  const vk::Viewport viewport{0.0f,
                              0.0f,
                              static_cast<float>(screenExtent.width),
                              static_cast<float>(screenExtent.height),
                              0.0f,
                              1.0f};
  currentCommandBuffer.setViewport(0, 1, &viewport);
  currentCommandBuffer.setScissor(0, 1, &extent);
  currentCommandBuffer.draw(3, 1, 0, 0);
  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();
  present(currentCommandBuffer, mOffscreenSemaphore,
          resources.imageRenderedSemaphore, resources.frameInUseFence,
          resources.imageIndex);
}

void DeferredRenderer::present(const vk::CommandBuffer& commandBuffer,
                               const vk::Semaphore& waitFor,
                               const vk::Semaphore& signalReady,
                               const vk::Fence& queueFence,
                               unsigned int imageIndex) {
  submit(commandBuffer, waitFor, signalReady, queueFence);
  const vk::Queue& queue = mContext->getQueue();

  auto const presentInfo =
      vk::PresentInfoKHR{}
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(&signalReady)
          .setSwapchainCount(1)
          .setPSwapchains(&mContext->getSwapchain()->getHandle())
          .setPImageIndices(&imageIndex);
  queue.presentKHR(&presentInfo);
}

void DeferredRenderer::submit(const vk::CommandBuffer& commandBuffer,
                              const vk::Semaphore& waitFor,
                              const vk::Semaphore& signalReady,
                              const vk::Fence& queueFence) {
  const vk::PipelineStageFlags pipelineStageFlags{
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  auto const submitInfo = vk::SubmitInfo{}
                              .setWaitSemaphoreCount(1)
                              .setPWaitSemaphores(&waitFor)
                              .setPWaitDstStageMask(&pipelineStageFlags)
                              .setCommandBufferCount(1)
                              .setPCommandBuffers(&commandBuffer)
                              .setSignalSemaphoreCount(1)
                              .setPSignalSemaphores(&signalReady);
  const vk::Queue& queue = mContext->getQueue();
  const vk::Result submitResult = queue.submit(1, &submitInfo, queueFence);
  assert(submitResult == vk::Result::eSuccess);
}

DeferredRenderer::~DeferredRenderer() {}