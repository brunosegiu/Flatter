﻿#include "rendering/vulkan/Instance.h"

using namespace Rendering::Vulkan;

VKAPI_ATTR VkBool32 VKAPI_CALL
Instance::VulkanReportFunc(VkDebugReportFlagsEXT flags,
                           VkDebugReportObjectTypeEXT objType,
                           uint64_t obj,
                           size_t location,
                           int32_t code,
                           const char* layerPrefix,
                           const char* msg,
                           void* userData) {
  printf("VULKAN VALIDATION: %s\n", msg);
  return VK_FALSE;
}

Instance::Instance() {
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
  const std::vector<const char*> extensions{VK_KHR_SURFACE_EXTENSION_NAME,
                                            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                                            VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
  createInfo.enabledExtensionCount = extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();
  const std::vector<const char*> layers{"VK_LAYER_LUNARG_standard_validation"};
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

const std::vector<VkPhysicalDevice> Instance::getAvailablePhisicalDevices()
    const {
  unsigned int physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(mInstanceHandle, &physicalDeviceCount, 0);
  std::vector<VkPhysicalDevice> physicalDeviceHandles(physicalDeviceCount,
                                                      nullptr);
  vkEnumeratePhysicalDevices(mInstanceHandle, &physicalDeviceCount,
                             physicalDeviceHandles.data());
  return physicalDeviceHandles;
}

Instance::~Instance() {
  /*
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
  if (vkpfn_DestroyDebugReportCallbackEXT && debugCallback) {
    vkpfn_DestroyDebugReportCallbackEXT(mInstance, debugCallback, 0);
  }
#endif
  vkDestroyInstance(mInstance, 0);
}*/
}
