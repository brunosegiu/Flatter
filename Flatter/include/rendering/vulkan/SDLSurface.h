#pragma once

#include "rendering/vulkan/Surface.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "rendering/vulkan/Instance.h"

namespace Rendering {

namespace Vulkan {

class SDLSurface : public Surface {
 public:
  SDLSurface(SDL_Window* window, const Instance& vkInstance);
  virtual ~SDLSurface();

 private:
  VkSurfaceKHR mSurface;

  SDLSurface(const SDLSurface&) = delete;
};

}  // namespace Vulkan
}  // namespace Rendering