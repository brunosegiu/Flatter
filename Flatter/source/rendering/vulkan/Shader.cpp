#include "rendering/vulkan/Shader.h"

#include <fstream>

using namespace Rendering::Vulkan;

ShaderRef Shader::fromFile(const std::string& path, const DeviceRef& device) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> rawData(fileSize);
  if (file.read(rawData.data(), fileSize)) {
    ShaderRef shader = std::make_shared<Shader>(device, rawData, fileSize);
    return shader;
  }
  return nullptr;
}

Shader::Shader(const DeviceRef& device,
               const std::vector<char>& code,
               const size_t size)
    : mDevice(device) {
  VkShaderModuleCreateInfo shaderCreateInfo{};
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.codeSize = size;
  shaderCreateInfo.pCode = reinterpret_cast<const unsigned int*>(code.data());
  const VkDevice& deviceHandle = device->getHandle();
  vkCreateShaderModule(deviceHandle, &shaderCreateInfo, 0, &mShaderHandle);
}

Shader::~Shader() {
  const VkDevice& deviceHandle = mDevice->getHandle();
  vkDestroyShaderModule(deviceHandle, mShaderHandle, 0);
}
