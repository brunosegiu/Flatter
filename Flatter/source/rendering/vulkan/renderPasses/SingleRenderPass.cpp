#include "rendering/vulkan/renderpasses/SingleRenderPass.h"

using namespace Rendering::Vulkan;

SingleRenderPass::SingleRenderPass(const ContextRef& context,
                                   const vk::Format& surfaceFormat,
                                   const vk::Format& depthFormat)
    : RenderPass(context) {
  auto const colorAttachmentDescription =
      vk::AttachmentDescription{}
          .setFormat(surfaceFormat)
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

  auto const depthAttachmentDescription =
      vk::AttachmentDescription{}
          .setFormat(depthFormat)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto const depthAttachmentRef =
      vk::AttachmentReference{}.setAttachment(1).setLayout(
          vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto const subpassDescription =
      vk::SubpassDescription{}
          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
          .setColorAttachmentCount(1)
          .setPColorAttachments(&colorReference)
          .setPDepthStencilAttachment(&depthAttachmentRef);

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

  const std::vector<vk::AttachmentDescription> attachments{
      colorAttachmentDescription, depthAttachmentDescription};

  auto const renderPassCreateInfo =
      vk::RenderPassCreateInfo{}
          .setAttachmentCount(static_cast<unsigned int>(attachments.size()))
          .setPAttachments(attachments.data())
          .setSubpassCount(1)
          .setPSubpasses(&subpassDescription)
          .setDependencyCount(1)
          .setPDependencies(&subpassDependency);

  const vk::Device& device = mContext->getDevice();
  assert(device.createRenderPass(&renderPassCreateInfo, nullptr,
                                 &mRenderPassHandle) == vk::Result::eSuccess);
}

SingleRenderPass::~SingleRenderPass() {}