#include "rendering/vulkan/pipelines/DeferredPipeline.h"

#include "rendering/vulkan/IndexedVertexBuffer.h"

using namespace Rendering::Vulkan;

DeferredPipeline::DeferredPipeline(const ContextRef& context,
                                   const DescriptorLayoutRef& descriptorLayout,
                                   const vk::Format& colorFormat,
                                   const vk::Format& depthFormat)
    : Pipeline(context) {
  mRenderPass =
      std::make_shared<GBufferRenderPass>(mContext, colorFormat, depthFormat);

  mVertexShader = Shader::fromFile("shaders/build/gbuffer.vert.spv", context,
                                   vk::ShaderStageFlagBits::eVertex);
  mFragmentShader = Shader::fromFile("shaders/build/gbuffer.frag.spv", context,
                                     vk::ShaderStageFlagBits::eFragment);

  const std::vector<vk::PipelineShaderStageCreateInfo> stages{
      mVertexShader->getStageInfo(), mFragmentShader->getStageInfo()};

  // Pipeline stages setup

  const auto vertexInputState =
      vk::PipelineVertexInputStateCreateInfo{}
          .setVertexBindingDescriptionCount(1)
          .setVertexAttributeDescriptionCount(1)
          .setPVertexBindingDescriptions(
              &IndexedVertexBuffer::sBindingDescription)
          .setPVertexAttributeDescriptions(
              &IndexedVertexBuffer::sAttributeDescription);

  auto const inputAssemblyState =
      vk::PipelineInputAssemblyStateCreateInfo{}
          .setTopology(vk::PrimitiveTopology::eTriangleList)
          .setPrimitiveRestartEnable(false);

  auto const rasterizationState =
      vk::PipelineRasterizationStateCreateInfo{}
          .setDepthClampEnable(false)
          .setRasterizerDiscardEnable(false)
          .setPolygonMode(vk::PolygonMode::eFill)
          .setCullMode(vk::CullModeFlagBits::eBack)
          .setFrontFace(vk::FrontFace::eCounterClockwise)
          .setDepthBiasEnable(false)
          .setDepthBiasConstantFactor(0.0f)
          .setDepthBiasClamp(0.0f)
          .setDepthBiasSlopeFactor(0.0f)
          .setLineWidth(1.0f);
  auto const viewportState = vk::PipelineViewportStateCreateInfo{}
                                 .setViewportCount(1)
                                 .setPViewports(0)
                                 .setScissorCount(1)
                                 .setPScissors(0);
  auto const multisampleState =
      vk::PipelineMultisampleStateCreateInfo{}
          .setRasterizationSamples(vk::SampleCountFlagBits::e1)
          .setSampleShadingEnable(false)
          .setMinSampleShading(1.0f)
          .setPSampleMask(0)
          .setAlphaToCoverageEnable(false)
          .setAlphaToOneEnable(false);

  auto const depthStencilState = vk::PipelineDepthStencilStateCreateInfo{}
                                     .setDepthTestEnable(true)
                                     .setDepthWriteEnable(true)
                                     .setDepthCompareOp(vk::CompareOp::eLess)
                                     .setDepthBoundsTestEnable(false)
                                     .setStencilTestEnable(false);

  auto const colorblendAttachmentState =
      vk::PipelineColorBlendAttachmentState()
          .setBlendEnable(false)
          .setSrcColorBlendFactor(vk::BlendFactor::eOne)
          .setDstColorBlendFactor(vk::BlendFactor::eZero)
          .setColorBlendOp(vk::BlendOp::eAdd)
          .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
          .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
          .setAlphaBlendOp(vk::BlendOp::eAdd)
          .setColorWriteMask(
              vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

  auto const colorBlendState = vk::PipelineColorBlendStateCreateInfo{}
                                   .setLogicOpEnable(false)
                                   .setAttachmentCount(1)
                                   .setPAttachments(&colorblendAttachmentState);

  const std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport,
                                                    vk::DynamicState::eScissor};
  auto const dynamicState =
      vk::PipelineDynamicStateCreateInfo()
          .setDynamicStateCount(static_cast<unsigned int>(dynamicStates.size()))
          .setPDynamicStates(dynamicStates.data());
  auto const pipelineLayoutCreateInfo =
      vk::PipelineLayoutCreateInfo().setSetLayoutCount(1).setPSetLayouts(
          &descriptorLayout->getHandle());
  const vk::Device& device = mContext->getDevice();
  assert(device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr,
                                     &mPipelineLayout) == vk::Result::eSuccess);

  auto const pipelineCreateInfo =
      vk::GraphicsPipelineCreateInfo()
          .setStageCount(static_cast<unsigned int>(stages.size()))
          .setPStages(stages.data())
          .setPVertexInputState(&vertexInputState)
          .setPInputAssemblyState(&inputAssemblyState)
          .setPViewportState(&viewportState)
          .setPRasterizationState(&rasterizationState)
          .setPMultisampleState(&multisampleState)
          .setPColorBlendState(&colorBlendState)
          .setPDepthStencilState(&depthStencilState)
          .setPDynamicState(&dynamicState)
          .setLayout(mPipelineLayout)
          .setRenderPass(mRenderPass->getHandle());

  assert(device.createGraphicsPipelines(nullptr, 1, &pipelineCreateInfo,
                                        nullptr, &mPipelineHandle) ==
         vk::Result::eSuccess);
}

DeferredPipeline::~DeferredPipeline() {}