#pragma once

#include <memory>
#include <vector>

#include "rendering/Mesh.h"

namespace Rendering {
class Scene {
 public:
  Scene();

  virtual ~Scene();

 private:
  Scene(Scene const&) = delete;
  Scene& operator=(Scene const&) = delete;

  std::vector<MeshRef> mMeshes;
};

using SceneRef = std::shared_ptr<Scene>;

}  // namespace Rendering
