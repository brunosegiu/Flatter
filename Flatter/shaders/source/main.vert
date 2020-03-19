#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 0) uniform View {
    mat4 mvp;
} view;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 fragColor;

vec3 positions[3] = vec3[](
    vec3(0.0, -1.0, -1.0),
    vec3(-1.0, -1.0, 0.0),
    vec3(-1.0, 0.0, -1.0));

vec3 colors[3] = vec3[](
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 1.0)
);

void main()
{
    gl_Position = view.mvp * vec4(positions[gl_VertexIndex], 1.0);
    fragColor = colors[gl_VertexIndex];
}