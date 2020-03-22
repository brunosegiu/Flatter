#include "WindowManager.h"

#include "commons/Timer.h"
#include "input/CameraController.h"

using namespace Game;
using namespace Rendering::Vulkan;

WindowManager::WindowManager(const unsigned int width,
                             const unsigned int height)
    : mWidth(width), mHeight(height) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  mWindow = SDL_CreateWindow("Vulkan Sample", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width, height, 0);
  assert(mWindow != nullptr);

  mInstance = std::make_shared<Instance>();
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(mWindow, &info);
  mSurface = std::make_shared<Surface>(info, mInstance, width, height);
  mContext = std::make_shared<Context>(mInstance, mSurface);
  mRenderer = std::make_shared<Renderer>(mContext, mSurface);
}

void WindowManager::loop() {
  bool open = true;
  Input::CameraController cameraController(mWidth, mHeight);

  Commons::Timer timer;
  float timeDelta = 0.0f;

  do {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      open = !(evt.type == SDL_QUIT);
    }
    cameraController.process(timeDelta);
    mRenderer->draw(cameraController.getCamera());
    timer.end();
    timeDelta = timer.getDeltaMs();
    timer.restart();
  } while (open);
}

WindowManager ::~WindowManager() {
  const vk::Device& device = mContext->getDevice();
  device.waitIdle();
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}
