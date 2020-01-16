#include "WindowManager.h"

#include <assert.h>

WindowManager::WindowManager(const unsigned int width,
                             const unsigned int height,
                             const std::string appName)
    : mVkInstance(appName.c_str()) {
  assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) >= 0);
  mWindow = SDL_CreateWindow(appName.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width, height,
                             SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
  assert(mWindow);
}

void WindowManager::loop() {
  while (1) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return;
      }
    }
  }
}

WindowManager::~WindowManager() {
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}