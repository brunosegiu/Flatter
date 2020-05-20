#include "rendering/vulkan/renderPasses/GBufferRenderPass.h"

using namespace Rendering::Vulkan;

GBufferRenderPass::GBufferRenderPass(const ContextRef& context,
                                     const vk::Format& albedoFormat,
                                     const vk::Format& positionFormat,
                                     const vk::Format& normalFormat,
                                     const vk::Format& depthFormat)
    : RenderPass(context) {
  auto const colorAttachmentDescription =
      vk::AttachmentDescription{}
          .setFormat(albedoFormat)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

  auto const positionAttachmentDescription =
      vk::AttachmentDescription{}
          .setFormat(positionFormat)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

  auto const normalAttachmentDescription =
      vk::AttachmentDescription{}
          .setFormat(normalFormat)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

  const std::vector<vk::AttachmentReference> colorReference{
      vk::AttachmentReference{}
          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
          .setAttachment(0),
      vk::AttachmentReference{}
          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
          .setAttachment(1),
      vk::AttachmentReference{}
          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
          .setAttachment(2),
  };

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
      vk::AttachmentReference{}
          .setAttachment(static_cast<unsigned int>(colorReference.size()))
          .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto const subpassDescription =
      vk::SubpassDescription{}
          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
          .setColorAttachmentCount(
              static_cast<unsigned int>(colorReference.size()))
          .setPColorAttachments(colorReference.data())
          .setPDepthStencilAttachment(&depthReference);

  auto const layoutDependencies = std::vector<vk::SubpassDependency>{
      vk::SubpassDependency{}
          .setSrcSubpass(VK_SUBPASS_EXTERNAL)
          .setDstSubpass(0)
          .setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
          .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
          .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                            vk::AccessFlagBits::eColorAttachmentWrite)
          .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
      vk::SubpassDependency{}
          .setSrcSubpass(0)
          .setDstSubpass(VK_SUBPASS_EXTERNAL)
          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
          .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                            vk::AccessFlagBits::eColorAttachmentWrite)
          .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
          .setDependencyFlags(vk::DependencyFlagBits::eByRegion)};

  const std::vector<vk::AttachmentDescription> attachments{
      colorAttachmentDescription, positionAttachmentDescription,
      normalAttachmentDescription, depthAttachmentDescription};

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
