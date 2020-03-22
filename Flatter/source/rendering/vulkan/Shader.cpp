﻿#include "rendering/vulkan/Shader.h"

#include <fstream>

using namespace Rendering::Vulkan;

ShaderRef Shader::fromFile(const std::string& path, const ContextRef& context) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> rawData(fileSize);
  if (file.read(rawData.data(), fileSize)) {
    ShaderRef shader = std::make_shared<Shader>(context, rawData);
    return shader;
  }
  return nullptr;
}

Shader::Shader(const ContextRef& context, const std::vector<char>& code)
    : mContext(context) {
  auto const shaderCreateInfo =
      vk::ShaderModuleCreateInfo()
          .setCodeSize(code.size() * sizeof(char))
          .setPCode(reinterpret_cast<const unsigned int*>(code.data()));
  const vk::Device& device = mContext->getDevice();
  device.createShaderModule(&shaderCreateInfo, nullptr, &mShaderHandle);
}

Shader::~Shader() {
  const vk::Device& device = mContext->getDevice();
  device.destroyShaderModule(mShaderHandle, nullptr);
}
