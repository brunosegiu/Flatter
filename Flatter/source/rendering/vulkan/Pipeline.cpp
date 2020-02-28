#include "rendering/vulkan/Pipeline.h"

using namespace Rendering::Vulkan;

Pipeline::Pipeline(const DeviceRef device, const VkCommandBuffer& buffer) {
  mVertexShader = Shader::fromFile("shaders\\main.vert.spv");
  mFragmentShader = Shader::fromFile("shaders\\main.frag.spv");

  VkPipelineShaderStageCreateInfo mainVShaderInfo{};
  mainVShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  mainVShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  mainVShaderInfo.module = mVertexShader->getNativeHandle();
  mainVShaderInfo.pName = "main";

  VkPipelineShaderStageCreateInfo mainFShaderInfo{};
  mainFShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  mainFShaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  mainFShaderInfo.module = mFragmentShader->getNativeHandle();
  mainFShaderInfo.pName = "main";

  const std::vector<VkPipelineShaderStageCreateInfo> stages = {mainVShaderInfo,
                                                               mainFShaderInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputState{};
  vertexInputState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
  inputAssemblyState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyState.primitiveRestartEnable = VK_FALSE;

  VkPipelineRasterizationStateCreateInfo rasterizationState{};
  rasterizationState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationState.depthClampEnable = VK_FALSE;
  rasterizationState.rasterizerDiscardEnable = VK_FALSE;
  rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizationState.depthBiasEnable = VK_FALSE;
  rasterizationState.depthBiasConstantFactor = 0.0f;
  rasterizationState.depthBiasClamp = 0.0f;
  rasterizationState.depthBiasSlopeFactor = 0.0f;
  rasterizationState.lineWidth = 1.0f;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = 0;
  viewportState.scissorCount = 1;
  viewportState.pScissors = 0;

  VkPipelineMultisampleStateCreateInfo multisampleState{};
  multisampleState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleState.sampleShadingEnable = VK_FALSE;
  multisampleState.minSampleShading = 1.0f;
  multisampleState.pSampleMask = 0;
  multisampleState.alphaToCoverageEnable = VK_FALSE;
  multisampleState.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState blendAttachmentState{};
  blendAttachmentState.blendEnable = VK_FALSE;
  blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  blendAttachmentState.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlendState{};
  colorBlendState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendState.logicOpEnable = VK_FALSE;
  colorBlendState.attachmentCount = 1;
  colorBlendState.pAttachments = &blendAttachmentState;

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = 2;
  static auto dytnamicStates = std::vector<VkDynamicState>{
      VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  dynamicState.pDynamicStates = dytnamicStates.data();

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  const VkDevice& deviceHandle(mDevice->getNativeHandle());
  vkCreatePipelineLayout(deviceHandle, &pipelineLayoutCreateInfo, 0,
                         &pipelineLayout);

  static VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.stageCount = 2;
  pipelineCreateInfo.pStages = stages.data();
  pipelineCreateInfo.pVertexInputState = &vertexInputState;
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pRasterizationState = &rasterizationState;
  pipelineCreateInfo.pMultisampleState = &multisampleState;
  pipelineCreateInfo.pColorBlendState = &colorBlendState;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.layout = pipelineLayout;
  pipelineCreateInfo.renderPass = renderPass;

  vkCreateGraphicsPipelines(deviceHandle, VK_NULL_HANDLE, 1,
                            &pipelineCreateInfo, 0, &mPipelineHandle);

  vkDestroyShaderModule(deviceHandle, vertexShader, 0);
  vkDestroyShaderModule(deviceHandle, fragmentShader, 0);
}

void Pipeline::bind(const VkCommandBuffer& commandBuffer) {
  vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineHandle);
  VkViewport viewport{
      0.0f, 0.0f, (float)swapchainExtent.width, (float)swapchainExtent.height,
      0.0f, 1.0f};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  VkRect2D scissorRect{{0, 0}, swapchainExtent};
  vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
}

Pipeline::~Pipeline() {
  vkDestroyPipeline(device, pipeline, 0);
  vkDestroyPipelineLayout(device, pipelineLayout, 0);
}
