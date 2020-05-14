#include "rendering/vulkan/renderers/DeferredRenderer.h"

using namespace Rendering::Vulkan;

DeferredRenderer::DeferredRenderer(const ContextRef& context,
                                   const SurfaceRef& surface)
    : Renderer(context) {
  // GBuffer resources
  const vk::Device& device = mContext->getDevice();
  const vk::Extent2D extent(mContext->getSwapchain()->getExtent());
  mColorAtt = std::make_shared<FramebufferAttachment>(
      mContext, extent, vk::Format::eR8G8B8A8Unorm,
      vk::ImageUsageFlagBits::eColorAttachment |
          vk::ImageUsageFlagBits::eSampled,
      vk::ImageAspectFlagBits::eColor);
  mDepthAtt = std::make_shared<DepthBufferAttachment>(mContext, extent);
  mOffscreenRenderPass = std::make_shared<GBufferRenderPass>(
      mContext, mColorAtt->getFormat(), mDepthAtt->getFormat());
  std::vector<vk::ImageView> attachments = {mColorAtt->getImageView(),
                                            mDepthAtt->getImageView()};
  const auto gBufferInfo =
      vk::FramebufferCreateInfo{}
          .setRenderPass(mOffscreenRenderPass->getHandle())
          .setAttachmentCount(static_cast<unsigned int>(attachments.size()))
          .setPAttachments(attachments.data())
          .setWidth(extent.width)
          .setHeight(extent.height)
          .setLayers(1);
  const auto result = device.createFramebuffer(gBufferInfo);
  assert(result.result == vk::Result::eSuccess);
  mGBuffer = result.value;
  auto const commandBufferAllocInfo =
      vk::CommandBufferAllocateInfo{}
          .setCommandPool(mContext->getCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandBufferCount(1);
  device.allocateCommandBuffers(&commandBufferAllocInfo, &mDeferredCB);

  mDeferredPipeline = new DeferredPipeline(mContext, mOffscreenRenderPass);

  // Fullscreen pass resources
  mFullscreenRenderPass = std::make_shared<FullscreenRenderPass>(
      mContext, surface->getFormat(mContext->getPhysicalDevice()).format);

  mScreenFramebufferRing = std::make_shared<ScreenFramebufferRing>(
      mContext, surface, mFullscreenRenderPass, nullptr);

  mFullscreenPipeline =
      std::make_shared<FullscreenPipeline>(mContext, mFullscreenRenderPass);
}

void DeferredRenderer::draw(Rendering::Camera& camera, const SceneRef& scene) {
  const RenderingResources& resources = mScreenFramebufferRing->swapBuffers();
  const vk::CommandBuffer& currentCommandBuffer(resources.commandBuffer);
  auto const beginInfo = vk::CommandBufferBeginInfo{}.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  currentCommandBuffer.begin(&beginInfo);
  currentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                    mFullscreenPipeline->getHandle());
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

  const vk::PipelineStageFlags pipelineStageFlags{
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  auto const submitInfo =
      vk::SubmitInfo{}
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(&resources.imageAvailableSemaphore)
          .setPWaitDstStageMask(&pipelineStageFlags)
          .setCommandBufferCount(1)
          .setPCommandBuffers(&currentCommandBuffer)
          .setSignalSemaphoreCount(1)
          .setPSignalSemaphores(&resources.imageRenderedSemaphore);
  const vk::Queue& queue = mContext->getQueue();
  const vk::Result submitResult =
      queue.submit(1, &submitInfo, resources.frameInUseFence);
  assert(submitResult == vk::Result::eSuccess);
  auto const presentInfo =
      vk::PresentInfoKHR{}
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(&resources.imageRenderedSemaphore)
          .setSwapchainCount(1)
          .setPSwapchains(&mContext->getSwapchain()->getHandle())
          .setPImageIndices(&resources.imageIndex);
  queue.presentKHR(&presentInfo);
}

void DeferredRenderer::drawDeferred(Rendering::Camera& camera,
                                    const SceneRef& scene) {
  vk::Semaphore offscreenSemaphore =
      mContext->getDevice().createSemaphore(vk::SemaphoreCreateInfo{}).value;
  const auto cmdBeginInfo = vk::CommandBufferBeginInfo{};
  std::array<vk::ClearValue, 2> clearValues{};
  clearValues[0].setColor(
      vk::ClearColorValue(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f}));
  clearValues[1].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 1u));

  const vk::Extent2D screenExtent = mContext->getSwapchain()->getExtent();
  const vk::Rect2D extent{{0, 0}, screenExtent};

  const auto renderPassBeginInfo =
      vk::RenderPassBeginInfo{}
          .setRenderPass(mOffscreenRenderPass->getHandle())
          .setFramebuffer(mGBuffer)
          .setRenderArea(extent)
          .setClearValueCount(2)
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
  for (const auto& mesh : scene->getMeshes()) {
    const IndexedVertexBufferRef& indexedVertexBuffer =
        mesh->getIndexedVertexBuffer();
    vk::DeviceSize offset{0};
    mDeferredCB.bindVertexBuffers(0, 1, &indexedVertexBuffer->getVertexBuffer(),
                                  &offset);
    mDeferredCB.bindIndexBuffer(indexedVertexBuffer->getIndexBuffer(), offset,
                                vk::IndexType::eUint16);
    mDeferredCB.drawIndexed(indexedVertexBuffer->getIndexCount(), 1, 0, 0, 0);
  }
  mDeferredCB.endRenderPass();
  mDeferredCB.end();
}

DeferredRenderer::~DeferredRenderer() {}