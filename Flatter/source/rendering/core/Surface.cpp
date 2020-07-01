#include "rendering//core/Surface.h"

using namespace Rendering::Vulkan;

Surface::Surface(const SDL_SysWMinfo& info,
                 const InstanceRef& instance,
                 const unsigned int width,
                 const unsigned int height)
    : mInstance(instance), mWidth(width), mHeight(height) {
  auto const surfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR{}
                                     .setHinstance(GetModuleHandle(0))
                                     .setHwnd(info.info.win.window);

  assert(instance->createWin32SurfaceKHR(&surfaceCreateInfo, nullptr,
                                         &mSurfaceHandle) ==
         vk::Result::eSuccess);
}

const vk::SurfaceCapabilitiesKHR Surface::getCapabilities(
    const vk::PhysicalDevice& physicalDevice) const {
  vk::SurfaceCapabilitiesKHR surfaceCapabilities;
  assert(physicalDevice.getSurfaceCapabilitiesKHR(
             mSurfaceHandle, &surfaceCapabilities) == vk::Result::eSuccess);
  return surfaceCapabilities;
}

const vk::SurfaceFormatKHR& Surface::getFormat(
    const vk::PhysicalDevice& physicalDevice) const {
  uint32_t formatCount = 1;
  if (!mSurfaceFormat.has_value()) {
    this->mSurfaceFormat = vk::SurfaceFormatKHR();
    physicalDevice.getSurfaceFormatsKHR(
        mSurfaceHandle, &formatCount,
        static_cast<vk::SurfaceFormatKHR*>(nullptr));
    physicalDevice.getSurfaceFormatsKHR(mSurfaceHandle, &formatCount,
                                        &mSurfaceFormat.value());
    mSurfaceFormat->format = mSurfaceFormat->format == vk::Format::eUndefined
                                 ? vk::Format::eB8G8R8A8Unorm
                                 : mSurfaceFormat->format;
  }
  return mSurfaceFormat.value();
}

Surface::~Surface() {}