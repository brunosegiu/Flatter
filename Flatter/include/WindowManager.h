#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/Renderer.h"
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

 private:
  SDL_Window* mWindow;
  Rendering::Vulkan::Instance mInstance;
  Rendering::Vulkan::SDLSurfaceRef mSurface;
};
}  // namespace Game
