#pragma once

namespace Rendering {
namespace Vulkan {

class Surface {
 public:
  Surface() = default;
  virtual ~Surface() = default;

 private:
  Surface(const Surface&) = delete;
};

}  // namespace Vulkan
}  // namespace Rendering