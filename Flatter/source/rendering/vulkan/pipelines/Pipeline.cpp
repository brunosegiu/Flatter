#include "rendering/vulkan/pipelines/Pipeline.h"

using namespace Rendering::Vulkan;

Pipeline::Pipeline(const ContextRef& context) : mContext(context) {}

Pipeline::~Pipeline() {
  const vk::Device& device = mContext->getDevice();
  // device.destroyPipeline(mPipelineHandle, nullptr);
  // device.destroyPipelineLayout(mPipelineLayout, nullptr);
}
