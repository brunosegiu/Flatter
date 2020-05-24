#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform View {
    mat4 viewProjectionMatrix;
} view;

layout(push_constant) uniform PushConstants {
	mat4 modelMatrix;
} pushConstants;

layout (location = 0) out vec3 outFragPos;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    const mat4 viewProjection = view.viewProjectionMatrix;
    const mat4 model = pushConstants.modelMatrix;

    gl_Position = viewProjection * model * vec4(position, 1.0f);
    outFragPos = (model * vec4(position, 1.0f)).xyz;
    outFragPos.y = -outFragPos.y;
}