#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <memory>

#include "rendering/vulkan/Instance.h"
#include "rendering/vulkan/Surface.h"

namespace Rendering {

namespace Vulkan {

class SDLSurface : public Surface {
 public:
  SDLSurface(SDL_Window* window, const Instance& vkInstance);

  virtual ~SDLSurface();

 private:
  SDLSurface(const SDLSurface&) = delete;
};

using SDLSurfaceRef = std::shared_ptr<Rendering::Vulkan::SDLSurface>;

}  // namespace Vulkan
}  // namespace Rendering
