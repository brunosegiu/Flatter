#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 uv;

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D positionSampler;
layout (binding = 2) uniform sampler2D normalSampler;

layout(location = 0) out vec4 outColor;

vec3 lightPos = vec3(0,-3,0);

void main() {
    vec3 position = texture(positionSampler, uv).xyz;
    vec3 normal = texture(normalSampler, uv).xyz;
    vec3 albedo = texture(albedoSampler, uv).xyz;

	vec3 L = lightPos - position;
	// Distance from light to fragment position
	float dist = length(L);
	// Light to fragment
	L = normalize(L);
	// Attenuation
	float atten = 1000.0f / (pow(dist, 2.0) + 1.0);
	// Diffuse part
	vec3 N = normalize(normal);
	float NdotL = max(0.0, dot(N, L));
	vec3 diff = albedo.rgb * NdotL;
	outColor = vec4(albedo * NdotL,1.0f);	
}