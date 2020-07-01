#include "rendering//pipelines/SinglePassPipeline.h"

#include "rendering//IndexedVertexBuffer.h"

using namespace Rendering::Vulkan;

SinglePassPipeline::SinglePassPipeline(
    const ContextRef& context,
    const SingleRenderPassRef& renderPass,
    const DescriptorLayoutRef& DescriptorLayout)
    : Pipeline(context) {
  mVertexShader = Shader::fromFile("shaders/build/main.vert.spv", context,
                                   vk::ShaderStageFlagBits::eVertex);
  mFragmentShader = Shader::fromFile("shaders/build/main.frag.spv", context,
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
          .setPrimitiveRestartEnable(VK_FALSE);
  auto const rasterizationState =
      vk::PipelineRasterizationStateCreateInfo{}
          .setDepthClampEnable(VK_FALSE)
          .setRasterizerDiscardEnable(VK_FALSE)
          .setPolygonMode(vk::PolygonMode::eFill)
          .setCullMode(vk::CullModeFlagBits::eBack)
          .setFrontFace(vk::FrontFace::eCounterClockwise)
          .setDepthBiasEnable(VK_FALSE)
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
          .setSampleShadingEnable(VK_FALSE)
          .setMinSampleShading(1.0f)
          .setPSampleMask(0)
          .setAlphaToCoverageEnable(VK_FALSE)
          .setAlphaToOneEnable(VK_FALSE);

  auto const depthStencilState = vk::PipelineDepthStencilStateCreateInfo{}
                                     .setDepthTestEnable(true)
                                     .setDepthWriteEnable(true)
                                     .setDepthCompareOp(vk::CompareOp::eLess)
                                     .setDepthBoundsTestEnable(false)
                                     .setStencilTestEnable(false);

  auto const colorblendAttachmentState =
      vk::PipelineColorBlendAttachmentState()
          .setBlendEnable(VK_FALSE)
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
                                   .setLogicOpEnable(VK_FALSE)
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
          &DescriptorLayout->getHandle());
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
          .setRenderPass(renderPass->getHandle());

  assert(device.createGraphicsPipelines(nullptr, 1, &pipelineCreateInfo,
                                        nullptr, &mPipelineHandle) ==
         vk::Result::eSuccess);
}

SinglePassPipeline::~SinglePassPipeline() {}