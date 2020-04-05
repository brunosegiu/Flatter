#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform View {
    mat4 mvp;
} view;

layout (location = 0) out vec3 fragPosition;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = view.mvp * vec4(position, 1.0);
    fragPosition = gl_Position.xyz;
}