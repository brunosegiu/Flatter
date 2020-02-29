#include "rendering/vulkan/Shader.h"

#include <fstream>

using namespace Rendering::Vulkan;

Shader* Shader::fromFile(const std::string& path, const Device& device) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> rawData(fileSize);
  if (file.read(rawData.data(), fileSize)) {
    unsigned int* unsafeVectorPtr = (unsigned int*)(rawData.data());
    Shader* shader =
        new Shader(unsafeVectorPtr, fileSize / sizeof(unsigned int), device);
    return shader;
  }
  return nullptr;
}

Shader::Shader(unsigned int* code, const size_t size, const Device& device) {
  VkShaderModuleCreateInfo shaderCreateInfo{};
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.codeSize = size;
  shaderCreateInfo.pCode = code;
  vkCreateShaderModule(device.mDeviceHandle, &shaderCreateInfo, 0,
                       &mShaderHandle);
}

Shader ::~Shader() {
  // vkDestroyShaderModule(device, fragmentShader, 0);
}
