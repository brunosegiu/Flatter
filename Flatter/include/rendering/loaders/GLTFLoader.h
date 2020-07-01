#pragma once

#include "Entity.h"
#include "rendering//core/Context.h"

namespace Rendering {
class GLTFLoader {
 public:
  GLTFLoader(const Rendering::Vulkan::ContextRef& context);

  std::vector<EntityRef> load(std::string path);

  virtual ~GLTFLoader() = default;

 private:
  const Rendering::Vulkan ::ContextRef mContext;
};
}  // namespace Rendering
