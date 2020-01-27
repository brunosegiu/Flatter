
#include <stdarg.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include <assert.h>
#include <vulkan/vulkan.h>
#include <vector>

#include "WindowManager.h"
#include "rendering/vulkan/Instance.h"

enum {
  MAX_DEVICE_COUNT = 8,
  MAX_QUEUE_COUNT = 4,  // ATM there should be at most transfer, graphics,
                        // compute, graphics+compute families
  MAX_PRESENT_MODE_COUNT = 6,  // At the moment in spec
  MAX_SWAPCHAIN_IMAGES = 3,
  FRAME_COUNT = 2,
  PRESENT_MODE_MAILBOX_IMAGE_COUNT = 3,
  PRESENT_MODE_DEFAULT_IMAGE_COUNT = 2,
};

static uint32_t clamp_u32(uint32_t value, uint32_t min, uint32_t max) {
  return value < min ? min : (value > max ? max : value);
}

//----------------------------------------------------------

VkApplicationInfo appInfo;

#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
VkInstanceCreateInfo createInfoLunarGValidation;
VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo;
VkDebugReportCallbackEXT debugCallback = VK_NULL_HANDLE;

#endif

VkInstanceCreateInfo createInfo{};
VkInstance instance{};
VkPhysicalDevice physicalDevice{};
uint32_t queueFamilyIndex;
VkQueue queue{};
VkDevice device{};
PFN_vkCreateDebugReportCallbackEXT vkpfn_CreateDebugReportCallbackEXT = 0;
PFN_vkDestroyDebugReportCallbackEXT vkpfn_DestroyDebugReportCallbackEXT = 0;
VkDeviceCreateInfo deviceCreateInfo{};

void fini_vulkan() {
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
  if (vkpfn_DestroyDebugReportCallbackEXT && debugCallback) {
    vkpfn_DestroyDebugReportCallbackEXT(instance, debugCallback, 0);
  }
#endif
  vkDestroyInstance(instance, 0);
}

//----------------------------------------------------------

SDL_Window* window = 0;
VkSurfaceKHR surface = VK_NULL_HANDLE;
uint32_t width = 1280;
uint32_t height = 720;

bool init_window() {}

void fini_window() {
  vkDestroySurfaceKHR(instance, surface, 0);
  SDL_DestroyWindow(window);
}

void fini_device() { vkDestroyDevice(device, 0); }

//----------------------------------------------------------

VkSwapchainKHR swapchain;
uint32_t swapchainImageCount;
VkImage swapchainImages[MAX_SWAPCHAIN_IMAGES];
VkExtent2D swapchainExtent;
VkSurfaceFormatKHR surfaceFormat;

bool init_swapchain() {
  // Use first available format
  uint32_t formatCount = 1;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       0);  // suppress validation layer
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       &surfaceFormat);
  surfaceFormat.format = surfaceFormat.format == VK_FORMAT_UNDEFINED
                             ? VK_FORMAT_B8G8R8A8_UNORM
                             : surfaceFormat.format;

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                            &presentModeCount, NULL);
  VkPresentModeKHR presentModes[MAX_PRESENT_MODE_COUNT];
  presentModeCount = presentModeCount > MAX_PRESENT_MODE_COUNT
                         ? MAX_PRESENT_MODE_COUNT
                         : presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                            &presentModeCount, presentModes);

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;  // always supported.
  for (uint32_t i = 0; i < presentModeCount; ++i) {
    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }
  }
  swapchainImageCount = presentMode == VK_PRESENT_MODE_MAILBOX_KHR
                            ? PRESENT_MODE_MAILBOX_IMAGE_COUNT
                            : PRESENT_MODE_DEFAULT_IMAGE_COUNT;

  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                            &surfaceCapabilities);

  swapchainExtent = surfaceCapabilities.currentExtent;
  if (swapchainExtent.width == UINT32_MAX) {
    swapchainExtent.width =
        clamp_u32(width, surfaceCapabilities.minImageExtent.width,
                  surfaceCapabilities.maxImageExtent.width);
    swapchainExtent.height =
        clamp_u32(height, surfaceCapabilities.minImageExtent.height,
                  surfaceCapabilities.maxImageExtent.height);
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = surface;
  swapChainCreateInfo.minImageCount = swapchainImageCount;
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent = swapchainExtent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.pNext = 0;

  VkResult result =
      vkCreateSwapchainKHR(device, &swapChainCreateInfo, 0, &swapchain);
  if (result != VK_SUCCESS) {
    return 0;
  }

  vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);
  vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount,
                          swapchainImages);

  return 1;
}

void fini_swapchain() { vkDestroySwapchainKHR(device, swapchain, 0); }

//----------------------------------------------------------

VkRenderPass renderPass;

int createRenderPass() {
  VkRenderPassCreateInfo renderPassCreateInfo{};

  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = 1;
  VkAttachmentDescription attachments{};
  attachments.format = surfaceFormat.format;
  attachments.samples = VK_SAMPLE_COUNT_1_BIT;
  attachments.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachments.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  renderPassCreateInfo.pAttachments = &attachments;
  renderPassCreateInfo.subpassCount = 1;
  VkSubpassDescription subpasses{};
  subpasses.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
  subpasses.colorAttachmentCount = 1;
  VkAttachmentReference colorAttachments{};
  colorAttachments.attachment = 0;
  colorAttachments.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  subpasses.pColorAttachments = &colorAttachments;
  renderPassCreateInfo.pSubpasses = &subpasses;
  renderPassCreateInfo.dependencyCount = 1;
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dependencyFlags = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  renderPassCreateInfo.pDependencies = &dependency;
  vkCreateRenderPass(device, &renderPassCreateInfo, 0, &renderPass);

  return 1;
}

void destroyRenderPass() { vkDestroyRenderPass(device, renderPass, NULL); }

VkImageView swapchainImageViews[MAX_SWAPCHAIN_IMAGES];
VkFramebuffer framebuffers[MAX_SWAPCHAIN_IMAGES];

int createFramebuffers() {
  for (uint32_t i = 0; i < swapchainImageCount; ++i) {
    VkImageViewCreateInfo icreateInfo{};
    icreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    icreateInfo.image = swapchainImages[i];
    icreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    icreateInfo.format = surfaceFormat.format;
    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;
    icreateInfo.subresourceRange = subresourceRange;

    vkCreateImageView(device, &icreateInfo, 0, &swapchainImageViews[i]);

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = &swapchainImageViews[i];
    framebufferCreateInfo.width = swapchainExtent.width;
    framebufferCreateInfo.height = swapchainExtent.height;
    framebufferCreateInfo.layers = 1;
    vkCreateFramebuffer(device, &framebufferCreateInfo, 0, &framebuffers[i]);
  }

  return 1;
}

void destroyFramebuffers() {
  for (uint32_t i = 0; i < swapchainImageCount; ++i) {
    vkDestroyFramebuffer(device, framebuffers[i], NULL);
    vkDestroyImageView(device, swapchainImageViews[i], NULL);
  }
}

VkShaderModule createShaderModule(const char* shaderFile) {
  VkShaderModule shaderModule = VK_NULL_HANDLE;

  HANDLE hFile = CreateFile(shaderFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE) return VK_NULL_HANDLE;

  LARGE_INTEGER size;
  GetFileSizeEx(hFile, &size);

  HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  CloseHandle(hFile);
  if (!hMapping) return VK_NULL_HANDLE;

  const uint32_t* data =
      (const uint32_t*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);

  VkShaderModuleCreateInfo shaderModuleCreateInfo{};

  shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleCreateInfo.codeSize = size.LowPart;
  shaderModuleCreateInfo.pCode = data;
  vkCreateShaderModule(device, &shaderModuleCreateInfo, 0, &shaderModule);

  UnmapViewOfFile(data);
  CloseHandle(hMapping);

  return shaderModule;
}

VkPipelineLayout pipelineLayout;
VkPipeline pipeline;

int createPipeline() {
  VkShaderModule vertexShader = createShaderModule("shaders\\main.vert.spv");
  VkShaderModule fragmentShader = createShaderModule("shaders\\main.frag.spv");

  VkPipelineShaderStageCreateInfo mainVShaderInfo{};
  mainVShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  mainVShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  mainVShaderInfo.module = vertexShader;
  mainVShaderInfo.pName = "main";

  VkPipelineShaderStageCreateInfo mainFShaderInfo{};
  mainFShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  mainFShaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  mainFShaderInfo.module = fragmentShader;
  mainFShaderInfo.pName = "main";

  const static std::vector<VkPipelineShaderStageCreateInfo> stages = {
      mainVShaderInfo, mainFShaderInfo};

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

  vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, 0, &pipelineLayout);

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

  vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, 0,
                            &pipeline);

  vkDestroyShaderModule(device, vertexShader, 0);
  vkDestroyShaderModule(device, fragmentShader, 0);

  return pipeline != 0;
}

void destroyPipeline() {
  vkDestroyPipeline(device, pipeline, 0);
  vkDestroyPipelineLayout(device, pipelineLayout, 0);
}

uint32_t frameIndex = 0;
VkCommandPool commandPool;
VkCommandBuffer commandBuffers[FRAME_COUNT];
VkFence frameFences[FRAME_COUNT];  // Create with VK_FENCE_CREATE_SIGNALED_BIT.
VkSemaphore imageAvailableSemaphores[FRAME_COUNT];
VkSemaphore renderFinishedSemaphores[FRAME_COUNT];

bool init_render() {
  VkCommandPoolCreateInfo commandPoolCreateInfo;
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

  vkCreateCommandPool(device, &commandPoolCreateInfo, 0, &commandPool);

  VkCommandBufferAllocateInfo commandBufferAllocInfo{};
  commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocInfo.commandPool = commandPool;
  commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocInfo.commandBufferCount = FRAME_COUNT;

  vkAllocateCommandBuffers(device, &commandBufferAllocInfo, commandBuffers);

  VkSemaphoreCreateInfo semaphoreCreateInfo = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  vkCreateSemaphore(device, &semaphoreCreateInfo, 0,
                    &imageAvailableSemaphores[0]);
  vkCreateSemaphore(device, &semaphoreCreateInfo, 0,
                    &imageAvailableSemaphores[1]);
  vkCreateSemaphore(device, &semaphoreCreateInfo, 0,
                    &renderFinishedSemaphores[0]);
  vkCreateSemaphore(device, &semaphoreCreateInfo, 0,
                    &renderFinishedSemaphores[1]);

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  vkCreateFence(device, &fenceCreateInfo, 0, &frameFences[0]);
  vkCreateFence(device, &fenceCreateInfo, 0, &frameFences[1]);

  createRenderPass();
  createFramebuffers();
  createPipeline();

  return 1;
}

void fini_render() {
  vkDeviceWaitIdle(device);
  destroyPipeline();
  destroyFramebuffers();
  destroyRenderPass();
  vkDestroyFence(device, frameFences[0], 0);
  vkDestroyFence(device, frameFences[1], 0);
  vkDestroySemaphore(device, renderFinishedSemaphores[0], 0);
  vkDestroySemaphore(device, renderFinishedSemaphores[1], 0);
  vkDestroySemaphore(device, imageAvailableSemaphores[0], 0);
  vkDestroySemaphore(device, imageAvailableSemaphores[1], 0);
  vkDestroyCommandPool(device, commandPool, 0);
}

void draw_frame() {
  uint32_t index = (frameIndex++) % FRAME_COUNT;
  vkWaitForFences(device, 1, &frameFences[index], VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &frameFences[index]);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
                        imageAvailableSemaphores[index], VK_NULL_HANDLE,
                        &imageIndex);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffers[index], &beginInfo);

  VkRenderPassBeginInfo beginInfo2{};
  beginInfo2.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  beginInfo2.renderPass = renderPass;
  beginInfo2.framebuffer = framebuffers[imageIndex];
  beginInfo2.clearValueCount = 1;
  VkClearValue clearValue{0.0f, 0.1f, 0.2f, 1.0f};
  beginInfo2.pClearValues = &clearValue;
  VkOffset2D offset;
  offset.x = 0;
  offset.y = 0;
  beginInfo2.renderArea.offset = offset;
  beginInfo2.renderArea.extent = swapchainExtent;

  vkCmdBeginRenderPass(commandBuffers[index], &beginInfo2,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline);
  VkViewport viewport{
      0.0f, 0.0f, (float)swapchainExtent.width, (float)swapchainExtent.height,
      0.0f, 1.0f};
  vkCmdSetViewport(commandBuffers[index], 0, 1, &viewport);
  VkRect2D scissorRect{{0, 0}, swapchainExtent};
  vkCmdSetScissor(commandBuffers[index], 0, 1, &scissorRect);
  vkCmdDraw(commandBuffers[index], 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffers[index]);

  vkEndCommandBuffer(commandBuffers[index]);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &imageAvailableSemaphores[index];
  submitInfo.pWaitDstStageMask =
      std::vector<VkPipelineStageFlags>{
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}
          .data();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[index];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &renderFinishedSemaphores[index];
  vkQueueSubmit(queue, 1, &submitInfo, frameFences[index]);

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderFinishedSemaphores[index];
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain;
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(queue, &presentInfo);
}

//----------------------------------------------------------

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  Game::WindowManager wm();
  assert(init_swapchain());
  assert(init_render());
  int run = 1;

  while (run) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type == SDL_QUIT) {
        run = 0;
      }
    }

    draw_frame();
  }

  fini_render();
  fini_swapchain();
  fini_device();
  fini_window();
  fini_vulkan();

  SDL_Quit();

  return 0;
}
