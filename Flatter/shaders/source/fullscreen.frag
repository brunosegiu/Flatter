#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 uv;

layout (binding = 0) uniform sampler2D albedoSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(texture(albedoSampler, uv).rgb, 1.0f);
}