#include "rendering/vulkan/SDLSurface.h"

#include <assert.h>

using namespace Rendering::Vulkan;

SDLSurface::SDLSurface(SDL_Window* window, const Instance& vkInstance) {
  assert(SDL_Vulkan_CreateSurface(window, vkInstance.getInternalInstance(),
                                  &mSurface));
}

SDLSurface::~SDLSurface() {}