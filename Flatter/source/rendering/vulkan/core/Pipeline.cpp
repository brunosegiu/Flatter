#include "rendering/vulkan/core/Pipeline.h"

using namespace Rendering::Vulkan;

Pipeline::Pipeline(const ContextRef& context,
                   const RenderPassRef& renderPass,
                   const vk::DescriptorSetLayout& descriptorSetLayout)
    : mContext(context) {
  mVertexShader = Shader::fromFile("shaders/build/main.vert.spv", context);
  mFragmentShader = Shader::fromFile("shaders/build/main.frag.spv", context);

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
  auto const rasterizationState = vk::PipelineRasterizationStateCreateInfo{}
                                      .setDepthClampEnable(VK_FALSE)
                                      .setRasterizerDiscardEnable(VK_FALSE)
                                      .setPolygonMode(vk::PolygonMode::eFill)
                                      .setCullMode(vk::CullModeFlagBits::eNone)
                                      .setFrontFace(vk::FrontFace::eClockwise)
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
          &descriptorSetLayout);
  const vk::Device& device = mContext->getDevice();
  assert(device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr,
                                     &mPipelineLayoutHandle) ==
         vk::Result::eSuccess);

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
          .setPDynamicState(&dynamicState)
          .setLayout(mPipelineLayoutHandle)
          .setRenderPass(renderPass->getHandle());

  assert(device.createGraphicsPipelines(nullptr, 1, &pipelineCreateInfo,
                                        nullptr, &mPipelineHandle) ==
         vk::Result::eSuccess);
}

Pipeline::~Pipeline() {
  const vk::Device& device = mContext->getDevice();
  // vkDestroyPipeline(deviceHandle, mPipelineHandle, nullptr);
  device.destroyPipelineLayout(mPipelineLayoutHandle, nullptr);
}
