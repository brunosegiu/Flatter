#include "rendering/vulkan/core/Instance.h"

#include <vulkan/vulkan.h>

#include <iostream>

using namespace Rendering::Vulkan;

#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
#include <string>
VKAPI_ATTR VkBool32 VKAPI_CALL
Instance::VulkanReportFunc(VkDebugReportFlagsEXT flags,
                           VkDebugReportObjectTypeEXT objType,
                           uint64_t obj,
                           size_t location,
                           int32_t code,
                           const char* layerPrefix,
                           const char* msg,
                           void* userData) {
  const std::string message =
      ">>> VULKAN Validation Error: " + std::string(msg);
  OutputDebugString(message.c_str());
  std::cout << message << std::endl;
  __debugbreak();
  return VK_FALSE;
}

#endif

Instance::Instance() {
  auto const appInfo = vk::ApplicationInfo{}
                           .setPApplicationName("Flatter")
                           .setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
                           .setPEngineName("Flatter Engine")
                           .setEngineVersion(VK_MAKE_VERSION(0, 0, 1))
                           .setApiVersion(VK_API_VERSION_1_0);

  auto imageViewCreateInfo =
      vk::InstanceCreateInfo().setPApplicationInfo(&appInfo);
#ifndef VULKAN_ENABLE_LUNARG_VALIDATION
  const std::vector<const char*> extensions{
      VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
  imageViewCreateInfo.setEnabledExtensionCount(extensions.size())
      .setPpEnabledExtensionNames(extensions.data());
#else
  const std::vector<const char*> extensions{VK_KHR_SURFACE_EXTENSION_NAME,
                                            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                                            VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
  imageViewCreateInfo
      .setEnabledExtensionCount(static_cast<unsigned int>(extensions.size()))
      .setPpEnabledExtensionNames(extensions.data());
  const std::vector<const char*> layers{"VK_LAYER_LUNARG_standard_validation"};
  imageViewCreateInfo
      .setEnabledLayerCount(static_cast<unsigned int>(layers.size()))
      .setPpEnabledLayerNames(layers.data());
#endif
  assert(vk::createInstance(&imageViewCreateInfo, nullptr, this) ==
         vk::Result::eSuccess);
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
  VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo;
  debugCallbackCreateInfo.sType =
      VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  debugCallbackCreateInfo.flags =
      VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  debugCallbackCreateInfo.pfnCallback = VulkanReportFunc;
  vkpfn_CreateDebugReportCallbackEXT =
      (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
          VkInstance(this), "vkCreateDebugReportCallbackEXT");
  vkpfn_DestroyDebugReportCallbackEXT =
      (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
          VkInstance(this), "vkDestroyDebugReportCallbackEXT");
  if (vkpfn_CreateDebugReportCallbackEXT &&
      vkpfn_DestroyDebugReportCallbackEXT) {
    vkpfn_CreateDebugReportCallbackEXT(
        VkInstance(this), &debugCallbackCreateInfo, 0, &debugCallback);
  }
#endif
}

const std::vector<vk::PhysicalDevice> Instance::getAvailablePhysicalDevices()
    const {
  unsigned int physicalDeviceCount = 0;
  enumeratePhysicalDevices(&physicalDeviceCount,
                           static_cast<vk::PhysicalDevice*>(nullptr));
  std::vector<vk::PhysicalDevice> physicalDeviceHandles(physicalDeviceCount,
                                                        vk::PhysicalDevice());
  enumeratePhysicalDevices(&physicalDeviceCount, physicalDeviceHandles.data());
  return physicalDeviceHandles;
}

Instance::~Instance() {
#ifdef VULKAN_ENABLE_LUNARG_VALIDATION
  if (debugCallback) {
    // this->destroyDebugReportCallbackEXT(debugCallback, nullptr);
  }
#endif
}