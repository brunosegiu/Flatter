#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "input/CameraController.h"
#include "input/InputController.h"
#include "input/InputEventListener.h"
#include "rendering//core/Context.h"
#include "rendering//core/Instance.h"
#include "rendering//core/Surface.h"
#include "rendering//core/Swapchain.h"
#include "rendering//renderers/DeferredRenderer.h"
#include "rendering//renderers/SinglePassRenderer.h"
#include "rendering/Scene.h"

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
  SinglePassRendererRef mRenderer;
  DeferredRendererRef mDeff;
  Rendering::SceneRef mScene;

  Input::CameraControllerRef mCameraController;

  const unsigned int mWidth;
  const unsigned int mHeight;
  bool mOpen;
};
}  // namespace Game
