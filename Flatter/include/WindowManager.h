#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "rendering/vulkan/Device.h"
#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/Renderer.h"
#include "rendering/vulkan/Surface.h"
#include "rendering/vulkan/Swapchain.h"

using namespace Rendering::Vulkan;

namespace Game {

class WindowManager {
 public:
  WindowManager(const unsigned int width, const unsigned int height);

  void loop();

  virtual ~WindowManager();

 private:
  SDL_Window* mWindow;
  InstanceRef mInstance;
  SurfaceRef mSurface;
  SingleDeviceRef mDevice;
  SwapchainRef mSwapchain;
  RendererRef mRenderer;
};

}  // namespace Game
