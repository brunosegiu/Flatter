#include "rendering//renderPasses/FullscreenRenderPass.h"

using namespace Rendering::Vulkan;

FullscreenRenderPass::FullscreenRenderPass(const ContextRef& context,
                                           const vk::Format& colorFormat)
    : RenderPass(context) {
  auto const colorAttachmentDescription =
      vk::AttachmentDescription{}
          .setFormat(colorFormat)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

  auto const colorReference =
      vk::AttachmentReference{}
          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
          .setAttachment(0);

  auto const subpassDescription =
      vk::SubpassDescription{}
          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
          .setColorAttachmentCount(1)
          .setPColorAttachments(&colorReference);

  auto const subpassDependency =
      vk::SubpassDependency{}
          .setSrcSubpass(VK_SUBPASS_EXTERNAL)
          .setDstSubpass(0)
          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setSrcAccessMask(vk::AccessFlagBits{})
          .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                            vk::AccessFlagBits::eColorAttachmentRead)
          .setDependencyFlags(vk::DependencyFlags{});

  auto const renderPassCreateInfo =
      vk::RenderPassCreateInfo{}
          .setAttachmentCount(1)
          .setPAttachments(&colorAttachmentDescription)
          .setSubpassCount(1)
          .setPSubpasses(&subpassDescription)
          .setDependencyCount(1)
          .setPDependencies(&subpassDependency);

  const vk::Device& device = mContext->getDevice();
  assert(device.createRenderPass(&renderPassCreateInfo, nullptr,
                                 &mRenderPassHandle) == vk::Result::eSuccess);
}

FullscreenRenderPass::~FullscreenRenderPass() {}