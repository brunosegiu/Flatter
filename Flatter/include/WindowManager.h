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
  SDL_Window* mWindow;
  Instance* mInstance;
  Surface* mSurface;
  Device* mDevice;
  Swapchain* mSwapchain;
  Renderer* mRenderer;

  WindowManager(const unsigned int width, const unsigned int height);

  void loop();

  virtual ~WindowManager();
};

}  // namespace Game
