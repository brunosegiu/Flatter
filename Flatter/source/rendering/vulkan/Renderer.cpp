#include "rendering/vulkan/Renderer.h"

using namespace Rendering::Vulkan;

Renderer::Renderer(const DeviceRef& device,
                   const SurfaceRef& surface,
                   const SwapchainRef& swapchain,
                   const unsigned int frameCount)
    : mFramebuffers(swapchain->getImageCount(), nullptr),
      mDevice(device),
      mSwapchain(swapchain),
      mCurrentFrameIndex(0),
      mFrameCount(frameCount),
      mCommandBufferHandles(frameCount, nullptr),
      mAvailableImageSemaphore(frameCount, nullptr),
      mFinishedRenderSemaphore(frameCount, nullptr),
      mFrameFenceHandles(frameCount, nullptr) {
  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = device->getQueueFamilyIndex();

  const VkDevice& deviceHandle = device->getHandle();
  vkCreateCommandPool(deviceHandle, &commandPoolCreateInfo, 0,
                      &mCommandPoolHandle);

  VkCommandBufferAllocateInfo commandBufferAllocInfo{};
  commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocInfo.commandPool = mCommandPoolHandle;
  commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocInfo.commandBufferCount = frameCount;

  vkAllocateCommandBuffers(deviceHandle, &commandBufferAllocInfo,
                           mCommandBufferHandles.data());

  const VkSemaphoreCreateInfo semaphoreCreateInfo = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (unsigned int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
    vkCreateSemaphore(deviceHandle, &semaphoreCreateInfo, 0,
                      &mAvailableImageSemaphore[frameIndex]);
    vkCreateSemaphore(deviceHandle, &semaphoreCreateInfo, 0,
                      &mFinishedRenderSemaphore[frameIndex]);
    vkCreateFence(deviceHandle, &fenceCreateInfo, 0,
                  &mFrameFenceHandles[frameIndex]);
  }

  mRenderPass = std::make_shared<RenderPass>(device, surface);
  for (size_t frameIndex = 0; frameIndex < mFramebuffers.size(); ++frameIndex) {
    mFramebuffers[frameIndex] =
        std::make_shared<Framebuffer>(swapchain->getImage(frameIndex), device,
                                      surface, swapchain, mRenderPass);
  }
  mPipeline = std::make_shared<Pipeline>(device, mRenderPass);
}

void Renderer::draw() {
  uint32_t frameIndex = (mCurrentFrameIndex++) % mFrameCount;
  const unsigned int imageIndex = mSwapchain->acquireNextImage(
      mFrameFenceHandles[frameIndex], mAvailableImageSemaphore[frameIndex]);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(mCommandBufferHandles[frameIndex], &beginInfo);

  mRenderPass->begin(mFramebuffers[imageIndex], mSwapchain,
                     mCommandBufferHandles[frameIndex]);
  mPipeline->bind(mCommandBufferHandles[frameIndex]);
  const VkExtent2D& swapchainExtent = mSwapchain->getExtent();
  VkViewport viewport{0.0f,
                      0.0f,
                      static_cast<float>(swapchainExtent.width),
                      static_cast<float>(swapchainExtent.height),
                      0.0f,
                      1.0f};

  vkCmdSetViewport(mCommandBufferHandles[frameIndex], 0, 1, &viewport);
  VkRect2D scissors{{0, 0}, swapchainExtent};
  vkCmdSetScissor(mCommandBufferHandles[frameIndex], 0, 1, &scissors);
  vkCmdDraw(mCommandBufferHandles[frameIndex], 3, 1, 0, 0);

  vkCmdEndRenderPass(mCommandBufferHandles[frameIndex]);

  vkEndCommandBuffer(mCommandBufferHandles[frameIndex]);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &mAvailableImageSemaphore[frameIndex];
  VkPipelineStageFlags pipelineStageFlags{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.pWaitDstStageMask = &pipelineStageFlags;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBufferHandles[frameIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &mFinishedRenderSemaphore[frameIndex];

  const VkQueue& queueHandle = mDevice->getQueueHandle();

  vkQueueSubmit(queueHandle, 1, &submitInfo, mFrameFenceHandles[frameIndex]);

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &mFinishedRenderSemaphore[frameIndex];
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &mSwapchain->getHandle();
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(queueHandle, &presentInfo);
}

Renderer::~Renderer() {
  const VkDevice& deviceHandle = mDevice->getHandle();
  for (VkFence& fenceHandle : mFrameFenceHandles) {
    vkDestroyFence(deviceHandle, fenceHandle, 0);
  }
  for (VkSemaphore& semaphore : mFinishedRenderSemaphore) {
    vkDestroySemaphore(deviceHandle, semaphore, nullptr);
  }
  for (VkSemaphore& semaphore : mAvailableImageSemaphore) {
    vkDestroySemaphore(deviceHandle, semaphore, nullptr);
  }
  vkDestroyCommandPool(deviceHandle, mCommandPoolHandle, 0);
}
