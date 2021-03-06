#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

// Constants
layout(constant_id = 0)  const uint LIGHT_COUNT = 1;

// In
layout(location = 0) in vec2 uv;

layout(push_constant) uniform PushConstants {
	vec3 viewPos;
} pushConstants;

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D positionSampler;
layout (binding = 2) uniform sampler2D normalSampler;

struct Light {
	vec3 position;
	vec4 color;
};

layout (set = 1, binding = 0) uniform LightUBO {
	Light[LIGHT_COUNT] lights;
} lighUBO;

// Out
layout(location = 0) out vec4 outColor;

void main() {
	vec3 position = texture(positionSampler, uv).xyz;
    vec3 normal = texture(normalSampler, uv).xyz;
    vec3 albedo = texture(albedoSampler, uv).xyz;
	
	vec3 color = albedo * 0.1f;

	for (int i = 0; i < LIGHT_COUNT; ++i) {
		const Light currLight = lighUBO.lights[i];
		float intensity = currLight.color.a;
		vec3 lightPos = currLight.position;
		vec3 lightColor = currLight.color.rgb;
		vec3 L = lightPos - position;
		float dist = length(L);

		vec3 V = pushConstants.viewPos - position;
		V = normalize(V);
		
		L = normalize(L);

		float atten = intensity / max(dist * dist, 0.01f);

		vec3 N = normal;
		float NdotL = max(0.0, dot(N, L));
		vec3 diff = albedo.rgb * NdotL * atten;

		vec3 R = reflect(-L, N);
		float NdotR = max(0.0, dot(R, V));
		vec3 spec = vec3( pow(NdotR, 16.0) * atten );

		color += diff + spec;
	}
	outColor = vec4(color, 1.0);
}

