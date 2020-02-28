#include "rendering/vulkan/RenderPass.h"

#include "rendering/vulkan/Framebuffer.h"

using namespace Rendering::Vulkan;

RenderPass::RenderPass(const DeviceRef device, const VkFormat attachmentFormat)
    : mDevice(device) {
  VkRenderPassCreateInfo renderPassCreateInfo{};

  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = 1;
  VkAttachmentDescription attachments{};
  attachments.format = attachmentFormat;
  attachments.samples = VK_SAMPLE_COUNT_1_BIT;
  attachments.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachments.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  renderPassCreateInfo.pAttachments = &attachments;
  renderPassCreateInfo.subpassCount = 1;
  VkSubpassDescription subpasses{};
  subpasses.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
  subpasses.colorAttachmentCount = 1;
  VkAttachmentReference colorAttachments{};
  colorAttachments.attachment = 0;
  colorAttachments.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  subpasses.pColorAttachments = &colorAttachments;
  renderPassCreateInfo.pSubpasses = &subpasses;
  renderPassCreateInfo.dependencyCount = 1;
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dependencyFlags = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  renderPassCreateInfo.pDependencies = &dependency;
  vkCreateRenderPass(mDevice->getNativeHandle(), &renderPassCreateInfo, 0,
                     &mRenderPassHandle);
}

void RenderPass::begin(const VkCommandBuffer& commandBufferHandle,
                       const FramebufferRef framebuffer) {
  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = mRenderPassHandle;
  renderPassBeginInfo.framebuffer = framebuffer->getNativeHandle();
  renderPassBeginInfo.clearValueCount = 1;
  VkClearValue clearValue{0.0f, 0.1f, 0.2f, 1.0f};
  renderPassBeginInfo.pClearValues = &clearValue;
  VkOffset2D offset{0, 0};
  renderPassBeginInfo.renderArea.offset = offset;
  renderPassBeginInfo.renderArea.extent = swapchainExtent;

  vkCmdBeginRenderPass(commandBufferHandle, &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::end(const VkCommandBuffer& commandBufferHandle) {
  vkCmdEndRenderPass(commandBufferHandle);
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(mDevice->getNativeHandle(), mRenderPassHandle, NULL);
}
