#include "rendering/vulkan/Surface.h"

using namespace Rendering::Vulkan;

Surface::Surface(const SDL_SysWMinfo& info,
                 const InstanceRef& instance,
                 const unsigned int width,
                 const unsigned int height)
    : mInstance(instance), mWidth(width), mHeight(height), mSurfaceFormat() {
  auto const surfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
                                     .setHinstance(GetModuleHandle(0))
                                     .setHwnd(info.info.win.window);

  assert(instance->createWin32SurfaceKHR(&surfaceCreateInfo, nullptr,
                                         &mSurfaceHandle) ==
         vk::Result::eSuccess);
}

const vk::SurfaceCapabilitiesKHR Surface::getCapabilities(
    const SingleDeviceRef& device) const {
  vk::SurfaceCapabilitiesKHR surfaceCapabilities;
  device->getPhysicalHandle().getSurfaceCapabilitiesKHR(mSurfaceHandle,
                                                        &surfaceCapabilities);
  return surfaceCapabilities;
}

const vk::SurfaceFormatKHR& Surface::getSurfaceFormat(
    const SingleDeviceRef& device) const {
  uint32_t formatCount = 1;
  if (!mSurfaceFormat.has_value()) {
    this->mSurfaceFormat = vk::SurfaceFormatKHR();
    const vk::PhysicalDevice& physicalDeviceHandle =
        device->getPhysicalHandle();
    physicalDeviceHandle.getSurfaceFormatsKHR(
        mSurfaceHandle, &formatCount,
        static_cast<vk::SurfaceFormatKHR*>(nullptr));
    physicalDeviceHandle.getSurfaceFormatsKHR(mSurfaceHandle, &formatCount,
                                              &mSurfaceFormat.value());
    mSurfaceFormat->format = mSurfaceFormat->format == vk::Format::eUndefined
                                 ? vk::Format::eB8G8R8A8Unorm
                                 : mSurfaceFormat->format;
  }
  return mSurfaceFormat.value();
}

Surface::~Surface() {}
