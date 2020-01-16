#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "rendering/vulkan/Instance.h"

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
  Rendering::Vulkan::Instance mVkInstance;
};