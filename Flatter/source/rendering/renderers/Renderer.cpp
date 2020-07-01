#include "rendering//renderers/Renderer.h"

using namespace Rendering::Vulkan;

Renderer::Renderer(const ContextRef& context) : mContext(context) {}

Renderer::~Renderer() {}