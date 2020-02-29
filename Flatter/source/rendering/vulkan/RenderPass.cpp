#include "rendering/vulkan/RenderPass.h"

using namespace Rendering::Vulkan;

RenderPass::RenderPass(const DeviceRef& device, const SurfaceRef& surface)
    : mDevice(device) {
  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = 1;
  VkAttachmentDescription attachmentDescription{};
  attachmentDescription.format = surface->getSurfaceFormat(device).format;
  attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
  attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  renderPassCreateInfo.pAttachments = &attachmentDescription;
  renderPassCreateInfo.subpassCount = 1;

  VkSubpassDescription subpassDescription{};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  VkAttachmentReference attachment{};
  attachment.attachment = 0;
  attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &attachment;

  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;

  VkSubpassDependency subpassDependency{};
  subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependency.dstSubpass = 0;
  subpassDependency.srcStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.srcAccessMask = 0;
  subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  subpassDependency.dependencyFlags = 0;
  renderPassCreateInfo.pDependencies = &subpassDependency;
  const VkDevice& deviceHandle = device->getHandle();

  vkCreateRenderPass(deviceHandle, &renderPassCreateInfo, 0,
                     &mRenderPassHandle);
}

void RenderPass::begin(const FramebufferRef& framebuffer,
                       const SwapchainRef& swapchain,
                       const VkCommandBuffer& command) {
  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = mRenderPassHandle;
  renderPassBeginInfo.framebuffer = framebuffer->getHandle();
  renderPassBeginInfo.clearValueCount = 1;
  const VkClearValue clearValue{0.0f, 0.1f, 0.2f, 1.0f};
  renderPassBeginInfo.pClearValues = &clearValue;
  const VkOffset2D offset{0, 0};
  renderPassBeginInfo.renderArea.offset = offset;
  renderPassBeginInfo.renderArea.extent = swapchain->getExtent();
  vkCmdBeginRenderPass(command, &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(mDevice->getHandle(), mRenderPassHandle, NULL);
}
