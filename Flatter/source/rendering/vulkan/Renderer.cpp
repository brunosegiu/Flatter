#include "rendering/vulkan/Renderer.h"

using namespace Rendering::Vulkan;

Renderer::Renderer(const Device& device,
                   Surface& surface,
                   const Swapchain& swapchain,
                   const unsigned int frameCount)
    : mFramebuffers(swapchain.mSwapchainImageCount, nullptr),
      mDevice(&device),
      mSwapchain(&swapchain),
      mCurrentFrameIndex(0),
      mFrameCount(frameCount),
      mCommandBufferHandles(frameCount, nullptr),
      mAvailableImageSemaphore(frameCount, nullptr),
      mFinishedRenderSemaphore(frameCount, nullptr),
      mFrameFenceHandles(frameCount, nullptr) {
  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = device.mQueueFamilyIndex;

  vkCreateCommandPool(device.mDeviceHandle, &commandPoolCreateInfo, 0,
                      &mCommandPoolHandle);

  VkCommandBufferAllocateInfo commandBufferAllocInfo{};
  commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocInfo.commandPool = mCommandPoolHandle;
  commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocInfo.commandBufferCount = frameCount;

  vkAllocateCommandBuffers(device.mDeviceHandle, &commandBufferAllocInfo,
                           mCommandBufferHandles.data());

  const VkSemaphoreCreateInfo semaphoreCreateInfo = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (unsigned int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
    vkCreateSemaphore(device.mDeviceHandle, &semaphoreCreateInfo, 0,
                      &mAvailableImageSemaphore[frameIndex]);
    vkCreateSemaphore(device.mDeviceHandle, &semaphoreCreateInfo, 0,
                      &mFinishedRenderSemaphore[frameIndex]);
    vkCreateFence(device.mDeviceHandle, &fenceCreateInfo, 0,
                  &mFrameFenceHandles[0]);
  }

  mRenderPass = new RenderPass(surface, device);
  for (size_t frameIndex = 0; frameIndex < mFramebuffers.size(); ++frameIndex) {
    mFramebuffers[frameIndex] =
        new Framebuffer(swapchain.mSwapchainImages[frameIndex], device, surface,
                        swapchain, *mRenderPass);
  }
  mPipeline = new Pipeline(device, *mRenderPass);
}

void Renderer::draw() {
  uint32_t chainIndex = (mCurrentFrameIndex++) % mFrameCount;
  vkWaitForFences(mDevice->mDeviceHandle, 1, &mFrameFenceHandles[chainIndex],
                  VK_TRUE, UINT64_MAX);
  vkResetFences(mDevice->mDeviceHandle, 1, &mFrameFenceHandles[chainIndex]);

  unsigned int imageIndex = 0;
  vkAcquireNextImageKHR(mDevice->mDeviceHandle, mSwapchain->mSwapchainHandle,
                        UINT64_MAX, mAvailableImageSemaphore[chainIndex],
                        VK_NULL_HANDLE, &imageIndex);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(mCommandBufferHandles[chainIndex], &beginInfo);

  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = mRenderPass->mRenderPassHandle;
  renderPassBeginInfo.framebuffer =
      mFramebuffers[imageIndex]->mFramebufferHandle;
  renderPassBeginInfo.clearValueCount = 1;
  const VkClearValue clearValue{0.0f, 0.1f, 0.2f, 1.0f};
  renderPassBeginInfo.pClearValues = &clearValue;
  const VkOffset2D offset{0, 0};
  renderPassBeginInfo.renderArea.offset = offset;
  renderPassBeginInfo.renderArea.extent = mSwapchain->mSwapchainExtent;
  vkCmdBeginRenderPass(mCommandBufferHandles[chainIndex], &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(mCommandBufferHandles[chainIndex],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mPipeline->mPipelineHandle);
  VkViewport viewport{0.0f,
                      0.0f,
                      (float)mSwapchain->mSwapchainExtent.width,
                      (float)mSwapchain->mSwapchainExtent.height,
                      0.0f,
                      1.0f};

  vkCmdSetViewport(mCommandBufferHandles[chainIndex], 0, 1, &viewport);
  VkRect2D scissors{{0, 0}, mSwapchain->mSwapchainExtent};
  vkCmdSetScissor(mCommandBufferHandles[chainIndex], 0, 1, &scissors);
  vkCmdDraw(mCommandBufferHandles[chainIndex], 3, 1, 0, 0);

  vkCmdEndRenderPass(mCommandBufferHandles[chainIndex]);

  vkEndCommandBuffer(mCommandBufferHandles[chainIndex]);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &mAvailableImageSemaphore[chainIndex];
  VkPipelineStageFlags pipelineStageFlags{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.pWaitDstStageMask = &pipelineStageFlags;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBufferHandles[chainIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &mFinishedRenderSemaphore[chainIndex];

  vkQueueSubmit(mDevice->mQueueHandle, 1, &submitInfo,
                mFrameFenceHandles[chainIndex]);

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &mFinishedRenderSemaphore[chainIndex];
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &mSwapchain->mSwapchainHandle;
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(mDevice->mQueueHandle, &presentInfo);
}

Renderer ::~Renderer() {
  /*vkDeviceWaitIdle(device);
  destroyPipeline();
  destroyFramebuffers();
  destroyRenderPass();
  vkDestroyFence(device, frameFences[0], 0);
  vkDestroyFence(device, frameFences[1], 0);
  vkDestroySemaphore(device, renderFinishedSemaphores[0], 0);
  vkDestroySemaphore(device, renderFinishedSemaphores[1], 0);
  vkDestroySemaphore(device, imageAvailableSemaphores[0], 0);
  vkDestroySemaphore(device, imageAvailableSemaphores[1], 0);
  vkDestroyCommandPool(device, commandPool, 0);*/
}
