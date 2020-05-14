#pragma once

#include "rendering/Mesh.h"

namespace Rendering {
class GLTFLoader {
 public:
  GLTFLoader(const Rendering::Vulkan::ContextRef& context);

  std::vector<MeshRef> load(std::string path);

  virtual ~GLTFLoader() = default;

 private:
  const Rendering::Vulkan ::ContextRef mContext;
};
}  // namespace Rendering
