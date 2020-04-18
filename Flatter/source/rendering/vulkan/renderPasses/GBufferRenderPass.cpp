#include "rendering/vulkan/renderPasses/GBufferRenderPass.h"

using namespace Rendering::Vulkan;

GBufferRenderPass::GBufferRenderPass(const ContextRef& context,
                                     const vk::Format& colorFormat,
                                     const vk::Format& depthFormat)
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
          .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

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

  auto const depthReference =
      vk::AttachmentReference{}.setAttachment(1).setLayout(
          vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto const subpassDescription =
      vk::SubpassDescription{}
          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
          .setColorAttachmentCount(1)
          .setPColorAttachments(&colorReference)
          .setPDepthStencilAttachment(&depthReference);

  auto const layoutDependencies = std::vector<vk::SubpassDependency>{
      vk::SubpassDependency{}
          .setSrcSubpass(VK_SUBPASS_EXTERNAL)
          .setDstSubpass(0)
          .setSrcStageMask(vk::PipelineStageFlagBits::eFragmentShader)
          .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setSrcAccessMask(vk::AccessFlagBits::eShaderRead)
          .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
          .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
      vk::SubpassDependency{}
          .setSrcSubpass(0)
          .setDstSubpass(VK_SUBPASS_EXTERNAL)
          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setSrcAccessMask(vk::AccessFlagBits{})
          .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                            vk::AccessFlagBits::eColorAttachmentRead)
          .setDependencyFlags(vk::DependencyFlags{})};

  const std::vector<vk::AttachmentDescription> attachments{
      colorAttachmentDescription, depthAttachmentDescription};

  auto const renderPassCreateInfo =
      vk::RenderPassCreateInfo{}
          .setAttachmentCount(static_cast<unsigned int>(attachments.size()))
          .setPAttachments(attachments.data())
          .setSubpassCount(1)
          .setPSubpasses(&subpassDescription)
          .setDependencyCount(
              static_cast<unsigned int>(layoutDependencies.size()))
          .setPDependencies(layoutDependencies.data());
  const vk::Device& device = mContext->getDevice();
  assert(device.createRenderPass(&renderPassCreateInfo, nullptr,
                                 &mRenderPassHandle) == vk::Result::eSuccess);
}

GBufferRenderPass::~GBufferRenderPass() {}
