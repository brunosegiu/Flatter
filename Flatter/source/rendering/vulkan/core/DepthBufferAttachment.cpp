﻿#include "rendering/vulkan/core/DepthBufferAttachment.h"

using namespace Rendering::Vulkan;

DepthBufferAttachment::DepthBufferAttachment(const ContextRef& context,
                                             const vk::Extent2D& extent)
    : FramebufferAttachment(context,
                            extent,
                            pickFormat(context),
                            vk::ImageUsageFlagBits::eDepthStencilAttachment,
                            pickFormat(context) == vk::Format::eD32Sfloat
                                ? vk::ImageAspectFlagBits::eDepth
                                : (vk::ImageAspectFlagBits::eDepth |
                                   vk::ImageAspectFlagBits::eStencil)) {
  mFormat = pickFormat(context);
}

const vk::Format DepthBufferAttachment::pickFormat(const ContextRef& context) {
  return context->findSupportedFormat(
      {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
       vk::Format::eD24UnormS8Uint},
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlags(
          vk::FormatFeatureFlagBits::eDepthStencilAttachment));
}

DepthBufferAttachment::~DepthBufferAttachment() {}
