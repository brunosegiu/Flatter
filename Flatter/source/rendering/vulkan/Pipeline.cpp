#include "rendering/vulkan/Pipeline.h"

using namespace Rendering::Vulkan;

Pipeline::Pipeline(const DeviceRef& device, const RenderPass& renderPass)
    : mDevice(device) {
  mVertexShader = Shader::fromFile("shaders\\main.vert.spv", device);
  mFragmentShader = Shader::fromFile("shaders\\main.frag.spv", device);

  VkPipelineShaderStageCreateInfo vertexShaderStage{};
  vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStage.module = mVertexShader->mShaderHandle;
  vertexShaderStage.pName = "main";

  VkPipelineShaderStageCreateInfo fragmentShaderStage{};
  fragmentShaderStage.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStage.module = mFragmentShader->mShaderHandle;
  fragmentShaderStage.pName = "main";

  const std::vector<VkPipelineShaderStageCreateInfo> stages{
      vertexShaderStage, fragmentShaderStage};

  // Pipeline stages setup

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

  VkPipelineColorBlendStateCreateInfo colorBlendState{};
  colorBlendState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendState.logicOpEnable = VK_FALSE;
  colorBlendState.attachmentCount = 1;
  VkPipelineColorBlendAttachmentState colorblendAttachmentState{};
  colorblendAttachmentState.blendEnable = VK_FALSE;
  colorblendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorblendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorblendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  colorblendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorblendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorblendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  colorblendAttachmentState.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendState.pAttachments = &colorblendAttachmentState;

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  const std::vector<VkDynamicState> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT,
                                                  VK_DYNAMIC_STATE_SCISSOR};
  dynamicState.dynamicStateCount = dynamicStates.size();
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  const VkDevice& deviceHandle = device->getHandle();
  vkCreatePipelineLayout(deviceHandle, &pipelineLayoutCreateInfo, 0,
                         &mPipelineLayoutHandle);

  VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
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
  pipelineCreateInfo.layout = mPipelineLayoutHandle;
  pipelineCreateInfo.renderPass = renderPass.mRenderPassHandle;

  vkCreateGraphicsPipelines(deviceHandle, VK_NULL_HANDLE, 1,
                            &pipelineCreateInfo, 0, &mPipelineHandle);
}

void Pipeline::bind(const VkCommandBuffer& command) {
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineHandle);
}

Pipeline ::~Pipeline() {
  const VkDevice& deviceHandle = mDevice->getHandle();
  vkDestroyPipeline(deviceHandle, mPipelineHandle, nullptr);
  vkDestroyPipelineLayout(deviceHandle, mPipelineLayoutHandle, nullptr);
}
