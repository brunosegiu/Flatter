#include "rendering/vulkan/Surface.h"

using namespace Rendering::Vulkan;

Surface::Surface(const SDL_SysWMinfo& info,
                 const InstanceRef& instance,
                 const unsigned int width,
                 const unsigned int height)
    : mInstance(instance), mWidth(width), mHeight(height) {
  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
      VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
  surfaceCreateInfo.hinstance = GetModuleHandle(0);
  surfaceCreateInfo.hwnd = info.info.win.window;

  VkResult result = vkCreateWin32SurfaceKHR(
      instance->getHandle(), &surfaceCreateInfo, NULL, &mSurfaceHandle);
}

const VkSurfaceCapabilitiesKHR Surface::getCapabilities(
    const DeviceRef& device) const {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      device->getPhysicalHandle(), mSurfaceHandle, &surfaceCapabilities);
  return surfaceCapabilities;
}

const VkSurfaceFormatKHR Surface::getSurfaceFormat(const DeviceRef& device) {
  uint32_t formatCount = 1;
  if (surfaceFormat == nullptr) {
    this->surfaceFormat = new VkSurfaceFormatKHR();
    const VkPhysicalDevice& deviceHandle = device->getPhysicalHandle();
    vkGetPhysicalDeviceSurfaceFormatsKHR(deviceHandle, mSurfaceHandle,
                                         &formatCount,
                                         0);  // suppress validation layer
    vkGetPhysicalDeviceSurfaceFormatsKHR(deviceHandle, mSurfaceHandle,
                                         &formatCount, surfaceFormat);
    surfaceFormat->format = surfaceFormat->format == VK_FORMAT_UNDEFINED
                                ? VK_FORMAT_B8G8R8A8_UNORM
                                : surfaceFormat->format;
  }
  return *surfaceFormat;
}

Surface ::~Surface() {
  vkDestroySurfaceKHR(mInstance->getHandle(), mSurfaceHandle, nullptr);
}
