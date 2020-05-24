#include "WindowManager.h"

#include <iostream>

#include "rendering/loaders/GLTFLoader.h"

using namespace Game;
using namespace Rendering::Vulkan;

WindowManager::WindowManager(const unsigned int width,
                             const unsigned int height)
    : mWidth(width), mHeight(height), mOpen(true) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  mWindow = SDL_CreateWindow("Flatter", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width, height, 0);
  assert(mWindow != nullptr);

  mInstance = std::make_shared<Instance>();
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(mWindow, &info);
  mSurface = std::make_shared<Surface>(info, mInstance, width, height);
  mContext = std::make_shared<Context>(mInstance, mSurface);
  mRenderer = std::make_shared<SinglePassRenderer>(mContext, mSurface);

  Rendering::GLTFLoader loader(mContext);
  mScene = std::make_unique<Rendering::Scene>();
  mScene->add(loader.load("assets/monkey.glb")[0]);
  mScene->add(loader.load("assets/cube.glb")[0]);

  mCameraController = std::make_shared<Input::CameraController>();

  mDeff = std::make_shared<DeferredRenderer>(mContext, mSurface);
}

void WindowManager::onQuit() {
  mOpen = false;
}

void WindowManager::update(const float timeDelta) {
  mScene->getMeshes()[0]->getTransform()->rotate(
      glm::vec3(0.0f, timeDelta / 200000.0f, 0.0f));
  mScene->getMeshes()[1]->getTransform()->translate(
      glm::vec3(0.0f, timeDelta / 10000000.0f, 0.0f));
  mDeff->draw(mCameraController->getCamera(), mScene);
  mScene->getMeshes()[1]->getTransform()->rotate(
      glm::vec3(0.0f, 0.0f, timeDelta / 200000.0f));
}

WindowManager ::~WindowManager() {
  const vk::Device& device = mContext->getDevice();
  device.waitIdle();

  mRenderer.reset();
  delete mRenderer.get();

  mContext.reset();
  delete mContext.get();

  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}