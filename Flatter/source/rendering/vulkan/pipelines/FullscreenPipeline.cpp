#include "rendering/vulkan/pipelines/FullscreenPipeline.h"

using namespace Rendering::Vulkan;

FullscreenPipeline::FullscreenPipeline(
    const ContextRef& context,
    const FullscreenRenderPassRef& renderPass)
    : Pipeline(context) {
  mVertexShader =
      Shader::fromFile("shaders/build/fullscreen.vert.spv", context);
  mFragmentShader =
      Shader::fromFile("shaders/build/fullscreen.frag.spv", context);

  auto const vertexShaderStage = vk::PipelineShaderStageCreateInfo{}
                                     .setStage(vk::ShaderStageFlagBits::eVertex)
                                     .setModule(mVertexShader->getHandle())
                                     .setPName("main");

  auto const fragmentShaderStage =
      vk::PipelineShaderStageCreateInfo{}
          .setStage(vk::ShaderStageFlagBits::eFragment)
          .setModule(mFragmentShader->getHandle())
          .setPName("main");

  const std::vector<vk::PipelineShaderStageCreateInfo> stages{
      vertexShaderStage, fragmentShaderStage};

  // Pipeline stages setup

  const auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};

  auto const inputAssemblyState =
      vk::PipelineInputAssemblyStateCreateInfo{}
          .setTopology(vk::PrimitiveTopology::eTriangleList)
          .setPrimitiveRestartEnable(false);

  auto const rasterizationState = vk::PipelineRasterizationStateCreateInfo{}
                                      .setDepthClampEnable(false)
                                      .setRasterizerDiscardEnable(false)
                                      .setPolygonMode(vk::PolygonMode::eFill)
                                      .setCullMode(vk::CullModeFlagBits::eBack)
                                      .setFrontFace(vk::FrontFace::eClockwise)
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
                                     .setDepthTestEnable(false)
                                     .setDepthWriteEnable(false)
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
      vk::PipelineLayoutCreateInfo().setSetLayoutCount(0).setPSetLayouts(
          nullptr);
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

FullscreenPipeline::~FullscreenPipeline() {}