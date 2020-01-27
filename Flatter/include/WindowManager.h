#pragma once

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/SDLSurface.h"

namespace Game {

class WindowManager {
 public:
  WindowManager(const unsigned int width = 1280,
                const unsigned int height = 720,
                const std::string appName = "Unknown");
  WindowManager(const WindowManager&) = delete;

  void loop();

  virtual ~WindowManager();

  using SDLSurfaceRef = std::unique_ptr<Rendering::Vulkan::SDLSurface>;
  using DeviceRef = std::unique_ptr<Rendering::Vulkan::Device>;

 private:
  SDL_Window* mWindow;
  Rendering::Vulkan::Instance mInstance;
  SDLSurfaceRef mSurface;
  DeviceRef mDevice;
};
}  // namespace Game