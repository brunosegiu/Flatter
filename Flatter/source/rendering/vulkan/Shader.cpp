#include "rendering/vulkan/Shader.h"

#include <fstream>
#include <iterator>

using namespace Rendering::Vulkan;

const ShaderRef Shader::fromFile(const std::string path) {
  std::ifstream file(path, std::ios::binary);
  file.unsetf(std::ios::skipws);

  file.seekg(0, std::ios::end);
  const unsigned int fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<unsigned int> code(fileSize);
  code.insert(code.begin(), std::istream_iterator<unsigned int>(file),
              std::istream_iterator<unsigned int>());
  return std::make_shared<Shader>(code);
}

Shader::Shader(const DeviceRef device, const std::vector<unsigned int>& code)
    : mDevice(device) {
  VkShaderModuleCreateInfo shaderModuleCreateInfo{};

  shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleCreateInfo.codeSize = code.size() * sizeof(unsigned int);
  shaderModuleCreateInfo.pCode = code.data();
  vkCreateShaderModule(mDevice->getNativeHandle(), &shaderModuleCreateInfo, 0,
                       &mShaderModuleHandle);
}

Shader::~Shader() {
  const VkDevice deviceHandle(mDevice->getNativeHandle());
  vkDestroyShaderModule(deviceHandle, mShaderModuleHandle, NULL);
}
