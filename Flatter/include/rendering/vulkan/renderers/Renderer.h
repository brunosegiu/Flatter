#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/Camera.h"
#include "rendering/Scene.h"
#include "rendering/vulkan/ScreenFramebufferRing.h"
#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {
class Renderer {
 public:
  Renderer(const ContextRef& context);

  virtual void draw(Rendering::Camera& camera, const SceneRef& scene) = 0;

  virtual ~Renderer();

 protected:
  Renderer(Renderer const&) = delete;
  Renderer& operator=(Renderer const&) = delete;

  ScreenFramebufferRingRef mScreenFramebufferRing;

  ContextRef mContext;
};

using RendererRef = std::shared_ptr<Renderer>;
}  // namespace Vulkan
}  // namespace Rendering
