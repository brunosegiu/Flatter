#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "rendering/vulkan/core/Context.h"

namespace Rendering {
namespace Vulkan {
class Buffer;
using BufferRef = std::shared_ptr<Buffer>;

class Buffer {
 public:
  Buffer(const ContextRef& context,
         const size_t bufferSize,
         const vk::BufferUsageFlags usage,
         const vk::MemoryPropertyFlags memoryProperty);

  void copy(const BufferRef& target);
  void map(void* hostData);

  const vk::Buffer& getBuffer() const { return mBuffer; };

  virtual ~Buffer();

 private:
  Buffer(Buffer const&) = delete;
  Buffer& operator=(Buffer const&) = delete;

  vk::Buffer mBuffer;
  vk::DeviceMemory mBufferMemory;
  size_t mBufferSize;

  ContextRef mContext;
};

using BufferRef = std::shared_ptr<Buffer>;

}  // namespace Vulkan
}  // namespace Rendering
