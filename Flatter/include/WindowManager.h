#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "rendering/vulkan/Renderer.h"
#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Instance.h"
#include "rendering/vulkan/core/Surface.h"
#include "rendering/vulkan/core/Swapchain.h"

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
  ContextRef mContext;
  RendererRef mRenderer;

  const unsigned int mWidth;
  const unsigned int mHeight;
};

}  // namespace Game
