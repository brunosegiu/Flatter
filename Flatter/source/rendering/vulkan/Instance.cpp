#include "rendering/vulkan/Instance.h"

#include <SDL2/SDL_vulkan.h>
#include <assert.h>

#include <iostream>

#include "rendering/vulkan/Configuration.h"

using namespace Rendering::Vulkan;

VKAPI_ATTR VkBool32 VKAPI_CALL Instance::Report(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
    uint64_t obj, size_t location, int32_t code, const char* layerPrefix,
    const char* msg, void* userData) {
  std::cout << "VULKAN VALIDATION: " << msg << std::endl;
  return VK_FALSE;
}

Instance::Instance(const std::string appName) {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = nullptr;
  appInfo.pApplicationName = appName.c_str();
  appInfo.applicationVersion = 0;
  appInfo.pEngineName = appName.c_str();
  appInfo.engineVersion = 0;
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceInfo{};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo;

#ifndef VULKAN_ENABLE_VALIDATION
  const std::vector<const char*> extensions{
      VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
  instanceInfo.enabledExtensionCount =
      static_cast<unsigned int>(extensions.size());
  instanceInfo.ppEnabledExtensionNames = extensions.data();
#else
  const std::vector<const char*> validationLayers{
      "VK_LAYER_LUNARG_standard_validation"};
  instanceInfo.enabledLayerCount =
      static_cast<unsigned int>(validationLayers.size());
  instanceInfo.ppEnabledLayerNames = validationLayers.data();
  const std::vector<const char*> extensions{VK_KHR_SURFACE_EXTENSION_NAME,
                                            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                                            VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
  instanceInfo.enabledExtensionCount =
      static_cast<unsigned int>(extensions.size());

  instanceInfo.ppEnabledExtensionNames = extensions.data();

  mDebugCallbackCreate.sType =
      VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  mDebugCallbackCreate.flags =
      VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  mDebugCallbackCreate.pfnCallback = VulkanReportFunc;
#endif

  VkResult result = vkCreateInstance(&instanceInfo, nullptr, &mInstance);
  assert(result == VK_SUCCESS);
}

const VkInstance& Instance::getNativeHandle() const { return mInstance; }

Instance::~Instance() { vkDestroyInstance(mInstance, nullptr); }
