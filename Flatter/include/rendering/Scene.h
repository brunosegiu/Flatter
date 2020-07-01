#pragma once

#include <memory>
#include <vector>

#include "Entity.h"

namespace Rendering {
class Scene {
 public:
  Scene();

  void add(const EntityRef& entity);
  void add(const std::vector<EntityRef>& entities);

  const std::vector<EntityRef>& getEntities() const { return mEntities; };

  virtual ~Scene();

 private:
  Scene(Scene const&) = delete;
  Scene& operator=(Scene const&) = delete;

  std::vector<EntityRef> mEntities;
};

using SceneRef = std::shared_ptr<Scene>;
}  // namespace Rendering
