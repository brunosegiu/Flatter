#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable


layout(location = 0) in vec3 position;

layout(location = 0) out vec4 outColor;

void main() {
    const vec3 normal = normalize(cross(dFdx(position), dFdy(position)));
    const vec3 lightDir = normalize(vec3(0.0f, 1.0f, 0.5f));
    const float factor = dot(lightDir, normal);
    outColor = vec4(0.0f, 0.3f, 0.5f, 1.0f) * factor;
}