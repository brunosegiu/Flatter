#include "rendering//core/Shader.h"

#include <assert.h>

#include <fstream>

using namespace Rendering::Vulkan;

ShaderRef Shader::fromFile(const std::string& path,
                           const ContextRef& context,
                           const vk::ShaderStageFlagBits stage) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> rawData(fileSize);
  if (file.read(rawData.data(), fileSize)) {
    ShaderRef shader = std::make_shared<Shader>(context, rawData, stage);
    return shader;
  }
  return nullptr;
}

Shader::Shader(const ContextRef& context,
               const std::vector<char>& code,
               const vk::ShaderStageFlagBits stage)
    : mContext(context) {
  auto const shaderCreateInfo =
      vk::ShaderModuleCreateInfo{}
          .setCodeSize(code.size() * sizeof(char))
          .setPCode(reinterpret_cast<const unsigned int*>(code.data()));
  const vk::Device& device = mContext->getDevice();
  assert(device.createShaderModule(&shaderCreateInfo, nullptr,
                                   &mShaderHandle) == vk::Result::eSuccess);
  mStage = stage;
}

const vk::PipelineShaderStageCreateInfo Shader::getStageInfo() {
  return vk::PipelineShaderStageCreateInfo{}
      .setStage(mStage)
      .setModule(mShaderHandle)
      .setPName("main");
}

Shader::~Shader() {
  const vk::Device& device = mContext->getDevice();
  device.destroyShaderModule(mShaderHandle, nullptr);
}