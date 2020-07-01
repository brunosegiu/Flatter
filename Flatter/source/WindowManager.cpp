#include "WindowManager.h"

#include <iostream>

#include "rendering/loaders/GLTFLoader.h"
#include "terrain/Terrain.h"

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
  std::vector<EntityRef> entities = loader.load("assets/cube.glb");
  mScene->add(entities[0]);
  entities[0]->getTransform()->setPosition(glm::vec3(-25.0f, 0.0f, -25.0f));

  entities = loader.load("assets/cube.glb");
  mScene->add(entities[0]);
  entities[0]->getTransform()->setPosition(glm::vec3(25.0f, 0.0f, -25.0f));

  entities = loader.load("assets/cube.glb");
  mScene->add(entities[0]);
  entities[0]->getTransform()->setPosition(glm::vec3(-25.0f, 0.0f, 25.0f));

  entities = loader.load("assets/cube.glb");
  mScene->add(entities[0]);
  entities[0]->getTransform()->setPosition(glm::vec3(25.0f, 0.0f, 25.0f));

  mCameraController = std::make_shared<Input::CameraController>();

  EntityRef terrain = std::make_shared<Terrain>(mContext, glm::uvec2(30, 30),
                                                glm::vec2(50.0f, 50.0f));
  mScene->add(terrain);

  mDeff = std::make_shared<DeferredRenderer>(mContext, mSurface);
}

void WindowManager::onQuit() {
  mOpen = false;
}

void WindowManager::update(const float timeDelta) {
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