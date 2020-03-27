#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform View {
    mat4 mvp;
} view;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 fragColor;



vec3 rand(){
    return vec3(fract(sin(dot(vec3(1.0f) ,vec3(12.9898,78.233, 42.6436))) * 43758.5453), fract(sin(dot(vec3(1.0f) ,vec3(12.9898,78.233, 42.6436))) * 43758.5453), fract(sin(dot(vec3(1.0f) ,vec3(12.9898,78.233, 42.6436))) * 43758.5453));
}

void main()
{
    gl_Position = view.mvp * vec4(position, 1.0);
    fragColor = rand();
}