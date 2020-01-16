#include "rendering/vulkan/Instance.h"

#include <assert.h>

#include <SDL2/SDL_vulkan.h>

using namespace Rendering::Vulkan;

Instance::Instance(const std::string appName) {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = nullptr;
  appInfo.pApplicationName = appName.c_str();
  appInfo.applicationVersion = 0;
  appInfo.pEngineName = appName.c_str();
  appInfo.engineVersion = 0;
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceInfo = {};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo, instanceInfo.enabledLayerCount = 1;
  const char* ppEnabledLayerNames[] = {"VK_LAYER_LUNARG_standard_validation"};
  instanceInfo.ppEnabledLayerNames = ppEnabledLayerNames;
  instanceInfo.enabledExtensionCount = 3;
  const char* pInstExt[] = {VK_KHR_SURFACE_EXTENSION_NAME,
                            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                            VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
  instanceInfo.ppEnabledExtensionNames = pInstExt;

  VkResult result = vkCreateInstance(&instanceInfo, nullptr, &mInstance);
  assert(result == VK_SUCCESS);
}

const VkInstance& Instance::getInternalInstance() const { return mInstance; }

Instance::~Instance() { vkDestroyInstance(mInstance, nullptr); }