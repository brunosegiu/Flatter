#include "WindowManager.h"

using namespace Game;
using namespace Rendering::Vulkan;

WindowManager::WindowManager(const unsigned int width,
                             const unsigned int height) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  mWindow = SDL_CreateWindow("Vulkan Sample", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width, height, 0);
  assert(mWindow != nullptr);

  mInstance = std::make_shared<Instance>();
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(mWindow, &info);
  mSurface = std::make_shared<Surface>(info, mInstance, width, height);
  mDevice = std::make_shared<Device>(mInstance, mSurface);
  mSwapchain = std::make_shared<Swapchain>(mDevice, mSurface);
  mRenderer = std::make_shared<Renderer>(mDevice, mSurface, mSwapchain, 2);
}

void WindowManager::loop() {
  bool open = true;
  do {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      open = !(evt.type == SDL_QUIT);
    }
    mRenderer->draw();
  } while (open);
  SDL_Quit();
}

WindowManager ::~WindowManager() {
  SDL_DestroyWindow(mWindow);
}
