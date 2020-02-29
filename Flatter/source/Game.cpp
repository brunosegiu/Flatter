#define VULKAN_ENABLE_LUNARG_VALIDATION

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <assert.h>
#include <stdarg.h>

#include <string>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

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

uint32_t width = 1280;
uint32_t height = 720;

static uint32_t clamp_u32(uint32_t value, uint32_t min, uint32_t max) {
  return value < min ? min : (value > max ? max : value);
}

//----------------------------------------------------------

class Instance {
 public:
  VkInstance mInstanceHandle;

  static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanReportFunc(
      VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
      uint64_t obj, size_t location, int32_t code, const char* layerPrefix,
      const char* msg, void* userData) {
    printf("VULKAN VALIDATION: %s\n", msg);
    return VK_FALSE;
  }

  Instance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan SDL tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
#ifndef VULKAN_ENABLE_LUNARG_VALIDATION
    const std::vector<const char*> extensions{
        VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
#else
    const std::vector<const char*> extensions{
        VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    const std::vector<const char*> layers{
        "VK_LAYER_LUNARG_standard_validation"};
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
#endif
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    result = vkCreateInstance(&createInfo, 0, &mInstanceHandle);
    assert(result == VK_SUCCESS);
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
    PFN_vkCreateDebugReportCallbackEXT vkpfn_CreateDebugReportCallbackEXT = 0;
    PFN_vkDestroyDebugReportCallbackEXT vkpfn_DestroyDebugReportCallbackEXT = 0;
    VkDebugReportCallbackEXT debugCallback = VK_NULL_HANDLE;
    VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo;
    debugCallbackCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugCallbackCreateInfo.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debugCallbackCreateInfo.pfnCallback = VulkanReportFunc;
    vkpfn_CreateDebugReportCallbackEXT =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
            mInstanceHandle, "vkCreateDebugReportCallbackEXT");
    vkpfn_DestroyDebugReportCallbackEXT =
        (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
            mInstanceHandle, "vkDestroyDebugReportCallbackEXT");
    if (vkpfn_CreateDebugReportCallbackEXT &&
        vkpfn_DestroyDebugReportCallbackEXT) {
      vkpfn_CreateDebugReportCallbackEXT(
          mInstanceHandle, &debugCallbackCreateInfo, 0, &debugCallback);
    }
#endif
  }

  ~Instance() {
    /*
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
    if (vkpfn_DestroyDebugReportCallbackEXT && debugCallback) {
      vkpfn_DestroyDebugReportCallbackEXT(mInstance, debugCallback, 0);
    }
#endif
    vkDestroyInstance(mInstance, 0);
  }*/
  }
};

class Surface;
class Device {
 public:
  VkPhysicalDevice mPhysicalDeviceHandle;
  VkDevice mDeviceHandle;
  unsigned int mQueueFamilyIndex;
  VkQueue mQueueHandle;

  Device(const Instance& instance, const Surface& surface);
  ~Device();
};

class Surface {
 public:
  VkSurfaceKHR mSurfaceHandle;
  VkSurfaceFormatKHR* surfaceFormat;

  Surface(const SDL_SysWMinfo& info, const Instance& instance) {
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    surfaceCreateInfo.hinstance = GetModuleHandle(0);
    surfaceCreateInfo.hwnd = info.info.win.window;

    VkResult result = vkCreateWin32SurfaceKHR(
        instance.mInstanceHandle, &surfaceCreateInfo, NULL, &mSurfaceHandle);
  }

  const VkSurfaceCapabilitiesKHR getCapabilities(const Device& device) const {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device.mPhysicalDeviceHandle, mSurfaceHandle, &surfaceCapabilities);
    return surfaceCapabilities;
  }

  const VkSurfaceFormatKHR getSurfaceFormat(const Device& device) {
    uint32_t formatCount = 1;
    if (surfaceFormat == nullptr) {
      this->surfaceFormat = new VkSurfaceFormatKHR();
      vkGetPhysicalDeviceSurfaceFormatsKHR(device.mPhysicalDeviceHandle,
                                           mSurfaceHandle, &formatCount,
                                           0);  // suppress validation layer
      vkGetPhysicalDeviceSurfaceFormatsKHR(device.mPhysicalDeviceHandle,
                                           mSurfaceHandle, &formatCount,
                                           surfaceFormat);
      surfaceFormat->format = surfaceFormat->format == VK_FORMAT_UNDEFINED
                                  ? VK_FORMAT_B8G8R8A8_UNORM
                                  : surfaceFormat->format;
    }
    return *surfaceFormat;
  }

  ~Surface() {
    // vkDestroySurfaceKHR(mInstance->mInstanceHandle, mSurface, 0);
  }
};

Device::Device(const Instance& instance, const Surface& surface) {
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  const std::vector<const char*> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.enabledExtensionCount = extensions.size();
  deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

  uint32_t physicalDeviceCount;
  VkPhysicalDevice deviceHandles[MAX_DEVICE_COUNT];
  VkQueueFamilyProperties queueFamilyProperties[MAX_QUEUE_COUNT];

  vkEnumeratePhysicalDevices(instance.mInstanceHandle, &physicalDeviceCount, 0);
  physicalDeviceCount = physicalDeviceCount > MAX_DEVICE_COUNT
                            ? MAX_DEVICE_COUNT
                            : physicalDeviceCount;
  vkEnumeratePhysicalDevices(instance.mInstanceHandle, &physicalDeviceCount,
                             deviceHandles);

  for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(deviceHandles[i],
                                             &queueFamilyCount, NULL);
    queueFamilyCount =
        queueFamilyCount > MAX_QUEUE_COUNT ? MAX_QUEUE_COUNT : queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(
        deviceHandles[i], &queueFamilyCount, queueFamilyProperties);

    for (uint32_t j = 0; j < queueFamilyCount; ++j) {
      VkBool32 supportsPresent = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(
          deviceHandles[i], j, surface.mSurfaceHandle, &supportsPresent);

      if (supportsPresent &&
          (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
        mQueueFamilyIndex = j;
        mPhysicalDeviceHandle = deviceHandles[i];
        break;
      }
    }

    if (mPhysicalDeviceHandle) {
      break;
    }
  }
  VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
  deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfo.queueFamilyIndex = this->mQueueFamilyIndex;
  deviceQueueCreateInfo.queueCount = 1;
  deviceQueueCreateInfo.pQueuePriorities = std::vector<float>{1.0f}.data();
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
  VkResult result = vkCreateDevice(mPhysicalDeviceHandle, &deviceCreateInfo,
                                   nullptr, &mDeviceHandle);
  vkGetDeviceQueue(mDeviceHandle, mQueueFamilyIndex, 0, &mQueueHandle);
  assert(result == VK_SUCCESS);
}

Device::~Device() { vkDestroyDevice(mDeviceHandle, 0); }

class Swapchain {
 public:
  VkSwapchainKHR mSwapchainHandle;
  unsigned int mSwapchainImageCount;
  std::vector<VkImage> mSwapchainImages;  // MAX_SWAPCHAIN_IMAGES
  VkExtent2D mSwapchainExtent;

  Swapchain(const Device& device, Surface& surface) {
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.mPhysicalDeviceHandle,
                                              surface.mSurfaceHandle,
                                              &presentModeCount, NULL);
    VkPresentModeKHR presentModes[MAX_PRESENT_MODE_COUNT];
    presentModeCount = presentModeCount > MAX_PRESENT_MODE_COUNT
                           ? MAX_PRESENT_MODE_COUNT
                           : presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.mPhysicalDeviceHandle,
                                              surface.mSurfaceHandle,
                                              &presentModeCount, presentModes);

    VkPresentModeKHR presentMode =
        VK_PRESENT_MODE_FIFO_KHR;  // always supported.
    for (uint32_t i = 0; i < presentModeCount; ++i) {
      if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        break;
      }
    }
    mSwapchainImageCount = presentMode == VK_PRESENT_MODE_MAILBOX_KHR
                               ? PRESENT_MODE_MAILBOX_IMAGE_COUNT
                               : PRESENT_MODE_DEFAULT_IMAGE_COUNT;

    const VkSurfaceCapabilitiesKHR surfaceCapabilities =
        surface.getCapabilities(device);

    mSwapchainExtent = surfaceCapabilities.currentExtent;
    if (mSwapchainExtent.width == UINT32_MAX) {
      mSwapchainExtent.width =
          clamp_u32(width, surfaceCapabilities.minImageExtent.width,
                    surfaceCapabilities.maxImageExtent.width);
      mSwapchainExtent.height =
          clamp_u32(height, surfaceCapabilities.minImageExtent.height,
                    surfaceCapabilities.maxImageExtent.height);
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};

    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surface.mSurfaceHandle;
    swapChainCreateInfo.minImageCount = mSwapchainImageCount;
    const VkSurfaceFormatKHR surfaceFormat = surface.getSurfaceFormat(device);
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = mSwapchainExtent;
    swapChainCreateInfo.imageArrayLayers = 1;  // 2 for stereo
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;
    VkResult result = vkCreateSwapchainKHR(
        device.mDeviceHandle, &swapChainCreateInfo, 0, &mSwapchainHandle);
    assert(result == VK_SUCCESS);
    vkGetSwapchainImagesKHR(device.mDeviceHandle, mSwapchainHandle,
                            &mSwapchainImageCount, NULL);
    mSwapchainImages = std::vector<VkImage>(mSwapchainImageCount, nullptr);
    vkGetSwapchainImagesKHR(device.mDeviceHandle, mSwapchainHandle,
                            &mSwapchainImageCount, mSwapchainImages.data());
  }

  ~Swapchain() {
    //  vkDestroySwapchainKHR(device, mSwapchainHandle, 0);
  }
};

class RenderPass {
 public:
  VkRenderPass mRenderPassHandle;

  RenderPass(Surface& surface, const Device& device) {
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    VkAttachmentDescription attachmentDescription{};
    attachmentDescription.format = surface.getSurfaceFormat(device).format;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.subpassCount = 1;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    VkAttachmentReference attachment{};
    attachment.attachment = 0;
    attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachment;

    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    vkCreateRenderPass(device.mDeviceHandle, &renderPassCreateInfo, 0,
                       &mRenderPassHandle);
  }

  ~RenderPass() {
    // vkDestroyRenderPass(device, renderPass, NULL);
  }
};

class Framebuffer {
 public:
  VkImageView mSwapchainImageViewHandle;
  VkFramebuffer mFramebufferHandle;

  Framebuffer(const VkImage& swapchainImage, const Device& device,
              Surface& surface, const Swapchain& swapchain,
              const RenderPass& renderPass) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapchainImage;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = surface.getSurfaceFormat(device).format;
    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;
    createInfo.subresourceRange = subresourceRange;
    vkCreateImageView(device.mDeviceHandle, &createInfo, 0,
                      &mSwapchainImageViewHandle);

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass.mRenderPassHandle;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = &mSwapchainImageViewHandle;
    framebufferCreateInfo.width = swapchain.mSwapchainExtent.width;
    framebufferCreateInfo.height = swapchain.mSwapchainExtent.height;
    framebufferCreateInfo.layers = 1;

    vkCreateFramebuffer(device.mDeviceHandle, &framebufferCreateInfo, 0,
                        &mFramebufferHandle);
  }

  ~Framebuffer() {
    // vkDestroyFramebuffer(device, framebuffers[i], NULL);
    // vkDestroyImageView(device, swapchainImageViews[i], NULL);
  }
};

class Shader {
 public:
  VkShaderModule mShaderHandle;

  static Shader* fromFile(const std::string& path, const Device& device) {
    HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                              OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return VK_NULL_HANDLE;

    LARGE_INTEGER size;
    GetFileSizeEx(hFile, &size);

    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    CloseHandle(hFile);
    if (!hMapping) return VK_NULL_HANDLE;

    const uint32_t* data =
        (const uint32_t*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    const size_t dataSize = size.LowPart;
    Shader* shader = new Shader(data, dataSize, device);
    UnmapViewOfFile(data);
    CloseHandle(hMapping);
    return shader;
  }

  Shader(const unsigned int* code, const size_t size, const Device& device) {
    VkShaderModuleCreateInfo shaderCreateInfo{};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = size;
    shaderCreateInfo.pCode = code;
    vkCreateShaderModule(device.mDeviceHandle, &shaderCreateInfo, 0,
                         &mShaderHandle);
  }

  ~Shader() {
    // vkDestroyShaderModule(device, fragmentShader, 0);
  }
};

class Pipeline {
 public:
  Shader* mVertexShader;
  Shader* mFragmentShader;
  VkPipeline mPipelineHandle;
  VkPipelineLayout mPipelineLayoutHandle;

  Pipeline(const Device& device, const RenderPass& renderPass) {
    mVertexShader = Shader::fromFile("shaders\\main.vert.spv", device);
    mFragmentShader = Shader::fromFile("shaders\\main.frag.spv", device);

    VkPipelineShaderStageCreateInfo vertexShaderStage{};
    vertexShaderStage.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
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

    vkCreatePipelineLayout(device.mDeviceHandle, &pipelineLayoutCreateInfo, 0,
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

    vkCreateGraphicsPipelines(device.mDeviceHandle, VK_NULL_HANDLE, 1,
                              &pipelineCreateInfo, 0, &mPipelineHandle);
  }

  ~Pipeline() {
    // vkDestroyPipeline(device, pipeline, 0);
    // vkDestroyPipelineLayout(device, pipelineLayout, 0);
  }
};

class Renderer {
 public:
  uint32_t mCurrentFrameIndex = 0;
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffers[FRAME_COUNT];
  VkFence
      frameFences[FRAME_COUNT];  // Create with VK_FENCE_CREATE_SIGNALED_BIT.
  VkSemaphore imageAvailableSemaphores[FRAME_COUNT];
  VkSemaphore renderFinishedSemaphores[FRAME_COUNT];

  RenderPass* mRenderPass;
  std::vector<Framebuffer*> mFramebuffers;
  Pipeline* mPipeline;

  const Device* mDevice;
  const Swapchain* mSwapchain;

  Renderer(const Device& device, Surface& surface, const Swapchain& swapchain)
      : mFramebuffers(swapchain.mSwapchainImageCount, nullptr),
        mDevice(&device),
        mSwapchain(&swapchain) {
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = device.mQueueFamilyIndex;

    vkCreateCommandPool(device.mDeviceHandle, &commandPoolCreateInfo, 0,
                        &commandPool);

    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool = commandPool;
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount = FRAME_COUNT;

    vkAllocateCommandBuffers(device.mDeviceHandle, &commandBufferAllocInfo,
                             commandBuffers);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    vkCreateSemaphore(device.mDeviceHandle, &semaphoreCreateInfo, 0,
                      &imageAvailableSemaphores[0]);
    vkCreateSemaphore(device.mDeviceHandle, &semaphoreCreateInfo, 0,
                      &imageAvailableSemaphores[1]);
    vkCreateSemaphore(device.mDeviceHandle, &semaphoreCreateInfo, 0,
                      &renderFinishedSemaphores[0]);
    vkCreateSemaphore(device.mDeviceHandle, &semaphoreCreateInfo, 0,
                      &renderFinishedSemaphores[1]);

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkCreateFence(device.mDeviceHandle, &fenceCreateInfo, 0, &frameFences[0]);
    vkCreateFence(device.mDeviceHandle, &fenceCreateInfo, 0, &frameFences[1]);

    mRenderPass = new RenderPass(surface, device);
    for (size_t frameIndex = 0; frameIndex < mFramebuffers.size();
         ++frameIndex) {
      mFramebuffers[frameIndex] =
          new Framebuffer(swapchain.mSwapchainImages[frameIndex], device,
                          surface, swapchain, *mRenderPass);
    }
    mPipeline = new Pipeline(device, *mRenderPass);
  }

  void draw() {
    uint32_t index = (mCurrentFrameIndex++) % FRAME_COUNT;
    vkWaitForFences(mDevice->mDeviceHandle, 1, &frameFences[index], VK_TRUE,
                    UINT64_MAX);
    vkResetFences(mDevice->mDeviceHandle, 1, &frameFences[index]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(mDevice->mDeviceHandle, mSwapchain->mSwapchainHandle,
                          UINT64_MAX, imageAvailableSemaphores[index],
                          VK_NULL_HANDLE, &imageIndex);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffers[index], &beginInfo);

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = mRenderPass->mRenderPassHandle;
    renderPassBeginInfo.framebuffer =
        mFramebuffers[imageIndex]->mFramebufferHandle;
    renderPassBeginInfo.clearValueCount = 1;
    const VkClearValue clearValue{0.0f, 0.1f, 0.2f, 1.0f};
    renderPassBeginInfo.pClearValues = &clearValue;
    const VkOffset2D offset{0, 0};
    renderPassBeginInfo.renderArea.offset = offset;
    renderPassBeginInfo.renderArea.extent = mSwapchain->mSwapchainExtent;
    vkCmdBeginRenderPass(commandBuffers[index], &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      mPipeline->mPipelineHandle);
    VkViewport viewport{0.0f,
                        0.0f,
                        (float)mSwapchain->mSwapchainExtent.width,
                        (float)mSwapchain->mSwapchainExtent.height,
                        0.0f,
                        1.0f};

    vkCmdSetViewport(commandBuffers[index], 0, 1, &viewport);
    VkRect2D scissors{{0, 0}, mSwapchain->mSwapchainExtent};
    vkCmdSetScissor(commandBuffers[index], 0, 1, &scissors);
    vkCmdDraw(commandBuffers[index], 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffers[index]);

    vkEndCommandBuffer(commandBuffers[index]);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[index];
    VkPipelineStageFlags pipelineStageFlags{
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[index];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores[index];

    vkQueueSubmit(mDevice->mQueueHandle, 1, &submitInfo, frameFences[index]);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[index];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &mSwapchain->mSwapchainHandle;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(mDevice->mQueueHandle, &presentInfo);
  }

  ~Renderer() {
    /*vkDeviceWaitIdle(device);
    destroyPipeline();
    destroyFramebuffers();
    destroyRenderPass();
    vkDestroyFence(device, frameFences[0], 0);
    vkDestroyFence(device, frameFences[1], 0);
    vkDestroySemaphore(device, renderFinishedSemaphores[0], 0);
    vkDestroySemaphore(device, renderFinishedSemaphores[1], 0);
    vkDestroySemaphore(device, imageAvailableSemaphores[0], 0);
    vkDestroySemaphore(device, imageAvailableSemaphores[1], 0);
    vkDestroyCommandPool(device, commandPool, 0);*/
  }
};

class WindowManager {
 public:
  SDL_Window* mWindow;
  Instance* mInstance;
  Surface* mSurface;
  Device* mDevice;
  Swapchain* mSwapchain;

  WindowManager() {
    mWindow = SDL_CreateWindow("Vulkan Sample", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height, 0);
    mInstance = new Instance();

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(mWindow, &info);
    mSurface = new Surface(info, *mInstance);
    mDevice = new Device(*mInstance, *mSurface);
    assert(mWindow != nullptr);
    mSwapchain = new Swapchain(*mDevice, *mSurface);
  }

  ~WindowManager() { SDL_DestroyWindow(mWindow); }
};

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  WindowManager window{};
  Renderer renderer(*window.mDevice, *window.mSurface, *window.mSwapchain);
  bool run = 1;
  while (run) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type == SDL_QUIT) {
        run = 0;
      }
    }

    renderer.draw();
  }

  SDL_Quit();

  return 0;
}
