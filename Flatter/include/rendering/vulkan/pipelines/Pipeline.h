#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {

class Pipeline {
 public:
  Pipeline(const ContextRef& context);

  const vk::Pipeline& getHandle() { return mPipelineHandle; };
  const vk::PipelineLayout& getPipelineLayout() const {
    return mPipelineLayout;
  };

  virtual ~Pipeline();

 protected:
  Pipeline(Pipeline const&) = delete;
  Pipeline& operator=(Pipeline const&) = delete;

  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mPipelineLayout;

  ContextRef mContext;
};

using PipelineRef = std::shared_ptr<Pipeline>;

}  // namespace Vulkan
}  // namespace Rendering
