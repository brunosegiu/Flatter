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
  mScene->add(loader.load("assets/scene.glb"));
  mScene->getMeshes()[2]->getTransform()->setPosition(
      glm::vec3(20.0f, -5.0f, 0.0f));
  mScene->getMeshes()[2]->getTransform()->setScale(
      glm::vec3(50.0f, 0.5f, 50.0f));
  mScene->getMeshes()[0]->getTransform()->setPosition(
      glm::vec3(10.0f, 3.0f, 3.0f));
  mScene->getMeshes()[1]->getTransform()->setPosition(
      glm::vec3(3.0f, 3.0f, -3.0f));
  mScene->getMeshes()[4]->getTransform()->setPosition(
      glm::vec3(3.0f, 3.0f, 3.0f));
  mScene->getMeshes()[3]->getTransform()->setPosition(
      glm::vec3(30000.0f, 3.0f, 3.0f));
  mCameraController = std::make_shared<Input::CameraController>();

  mDeff = std::make_shared<DeferredRenderer>(mContext, mSurface);
}

void WindowManager::onQuit() {
  mOpen = false;
}

void WindowManager::update(const float timeDelta) {
  mScene->getMeshes()[1]->getTransform()->rotate(
      glm::vec3(0.0f, timeDelta / 800000.0f, 0.0f));
  mDeff->draw(mCameraController->getCamera(), mScene);
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