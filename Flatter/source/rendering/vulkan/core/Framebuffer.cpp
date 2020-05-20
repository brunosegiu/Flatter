#include "rendering/vulkan/core/Framebuffer.h"

using namespace Rendering::Vulkan;

Framebuffer::Framebuffer(
    const ContextRef& context,
    const PipelineRef& pipeline,
    const std::vector<FramebufferAttachmentRef>& attachments,
    vk::Extent2D extent)
    : mContext(context) {
  std::vector<vk::ImageView> imageAttachments;
  imageAttachments.reserve(attachments.size());
  for (const FramebufferAttachmentRef& attachment : attachments) {
    imageAttachments.push_back(attachment->getImageView());
  }
  const auto framebufferCreateInfo =
      vk::FramebufferCreateInfo{}
          .setRenderPass(pipeline->getRenderPass()->getHandle())
          .setAttachmentCount(static_cast<unsigned int>(attachments.size()))
          .setPAttachments(imageAttachments.data())
          .setWidth(extent.width)
          .setHeight(extent.height)
          .setLayers(1);
  const auto samplerRes =
      mContext->getDevice().createFramebuffer(framebufferCreateInfo);
  assert(samplerRes.result == vk::Result::eSuccess);
  mFramebufferHandle = samplerRes.value;
}

Framebuffer::~Framebuffer() {}