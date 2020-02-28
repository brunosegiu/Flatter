#include "rendering/vulkan/Renderer.h"

using namespace Rendering::Vulkan;

Renderer::Renderer(const DeviceRef device, const SwapChainRef swapChain,
                   const unsigned int frameCount)
    : mDevice(device),
      mFrameCount(frameCount),
      mFrameIndex(0),
      mCommandBufferHandles(frameCount, nullptr),
      mFenceHandles(frameCount, nullptr),
      mRenderSemaphores(frameCount, nullptr),
      mSwapChain(swapChain) {
  const VkDevice& deviceHandle(mDevice->getNativeHandle());
  VkCommandPoolCreateInfo commandPoolCreateInfo;
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = mDevice->getQueueIndex();

  vkCreateCommandPool(deviceHandle, &commandPoolCreateInfo, 0,
                      &mCommandPoolHandle);

  VkCommandBufferAllocateInfo commandBufferAllocInfo{};
  commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocInfo.commandPool = mCommandPoolHandle;
  commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocInfo.commandBufferCount = frameCount;

  vkAllocateCommandBuffers(deviceHandle, &commandBufferAllocInfo,
                           mCommandBufferHandles.data());

  VkSemaphoreCreateInfo semaphoreCreateInfo = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  for (VkSemaphore& semaphoreHandle : mRenderSemaphores) {
    vkCreateSemaphore(deviceHandle, &semaphoreCreateInfo, 0, &semaphoreHandle);
  }

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (VkFence& fenceHandle : mFenceHandles) {
    vkCreateFence(deviceHandle, &fenceCreateInfo, 0, &fenceHandle);
  }
}

void Renderer::draw() {
  unsigned int frameIndex = (mFrameIndex++) % mFrameCount;
  const VkDevice& deviceHandle(mDevice->getNativeHandle());

  vkWaitForFences(deviceHandle, 1, &mFenceHandles[frameIndex], VK_TRUE,
                  UINT64_MAX);
  vkResetFences(deviceHandle, 1, &mFenceHandles[frameIndex]);

  mSwapChain->acquireImage(frameIndex);

  const VkCommandBuffer& activeCommandBuffer(mCommandBufferHandles[frameIndex]);
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(activeCommandBuffer, &beginInfo);

  mRenderPass->begin(activeCommandBuffer, mFrameBuffers[frameIndex]);
  mPipeline->bind();
  vkCmdDraw(activeCommandBuffer, 3, 1, 0, 0);
  mRenderPass->end(activeCommandBuffer);
  vkEndCommandBuffer(activeCommandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &mSwapChain->getImageSemaphore(frameIndex);
  submitInfo.pWaitDstStageMask =
      std::vector<VkPipelineStageFlags>{
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}
          .data();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &activeCommandBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &mRenderSemaphores[frameIndex];
  mDevice->submitCommand(submitInfo, mFenceHandles[frameIndex]);

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &mRenderSemaphores[frameIndex];
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &mSwapChain->getNativeHandle();
  presentInfo.pImageIndices = &frameIndex;
  mDevice->present(presentInfo);
}

Renderer::~Renderer() {
  const VkDevice& deviceHandle(mDevice->getNativeHandle());
  vkDeviceWaitIdle(deviceHandle);
  for (VkFence& fenceHandle : mFenceHandles) {
    vkDestroyFence(deviceHandle, fenceHandle, 0);
  }
  for (VkSemaphore& semaphoreHandle : mRenderSemaphores) {
    vkDestroySemaphore(deviceHandle, semaphoreHandle, 0);
  }
  vkDestroyCommandPool(deviceHandle, mCommandPoolHandle, 0);
}
