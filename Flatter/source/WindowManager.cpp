﻿#include "WindowManager.h"

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
}

void WindowManager::onQuit() {
  mOpen = false;
}

void WindowManager::update(const float timeDelta) {
  mRenderer->draw(mCameraController->getCamera(), mScene);
}

WindowManager ::~WindowManager() {
  const vk::Device& device = mContext->getDevice();
  device.waitIdle();
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}
