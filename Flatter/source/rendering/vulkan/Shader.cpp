#include "rendering/vulkan/Shader.h"

#include <fstream>

using namespace Rendering::Vulkan;

ShaderRef Shader::fromFile(const std::string& path,
                           const SingleDeviceRef& device) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> rawData(fileSize);
  if (file.read(rawData.data(), fileSize)) {
    ShaderRef shader = std::make_shared<Shader>(device, rawData);
    return shader;
  }
  return nullptr;
}

Shader::Shader(const SingleDeviceRef& device, const std::vector<char>& code)
    : mDevice(device) {
  auto const shaderCreateInfo =
      vk::ShaderModuleCreateInfo()
          .setCodeSize(code.size() * sizeof(char))
          .setPCode(reinterpret_cast<const unsigned int*>(code.data()));
  mDevice->createShaderModule(&shaderCreateInfo, nullptr, &mShaderHandle);
}

Shader::~Shader() {
  mDevice->destroyShaderModule(mShaderHandle, nullptr);
}
