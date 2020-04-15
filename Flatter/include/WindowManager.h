#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "input/CameraController.h"
#include "input/InputController.h"
#include "input/InputEventListener.h"
#include "rendering/Scene.h"
#include "rendering/vulkan/Renderer.h"
#include "rendering/vulkan/core/Context.h"
#include "rendering/vulkan/core/Instance.h"
#include "rendering/vulkan/core/Surface.h"
#include "rendering/vulkan/core/Swapchain.h"

using namespace Rendering::Vulkan;

namespace Game {

class WindowManager : public Input::InputEventListener,
                      public std::enable_shared_from_this<WindowManager> {
 public:
  WindowManager(const unsigned int width, const unsigned int height);

  void update(const float timeDelta);

  const Input::CameraControllerRef& getCameraController() const {
    return mCameraController;
  };

  void onQuit() override;

  SDL_Window* getSDLWindow() { return mWindow; };
  const bool isOpen() { return mOpen; };

  virtual ~WindowManager();

 private:
  SDL_Window* mWindow;
  InstanceRef mInstance;
  SurfaceRef mSurface;
  ContextRef mContext;
  RendererRef mRenderer;
  Rendering::SceneRef mScene;

  Input::CameraControllerRef mCameraController;

  const unsigned int mWidth;
  const unsigned int mHeight;
  bool mOpen;
};

}  // namespace Game
