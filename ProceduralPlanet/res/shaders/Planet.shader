#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 vCol;
out float dist;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;


void main()
{
	//vCol = vec4(clamp(position,0.0f,1.0f),1.0f);
	vCol = vec4(normal,1.0);
	dist = distance(vec3(0.0), position.xyz);
	//gl_Position = projectionMatrix *  modelMatrix * viewMatrix * vec4(position,1.0f);

	WorldPos = vec3(u_modelMatrix * vec4(position, 1.0));
	Normal = mat3(u_modelMatrix) * normal;

	gl_Position = u_projectionMatrix * u_viewMatrix * vec4(WorldPos, 1.0);
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;
in vec3 WorldPos;
in vec3 Normal;

in vec4 vCol;
in float dist;

// material parameters
uniform vec3  u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;

// lights
uniform vec3 u_lightPositions[1];
uniform vec3 u_lightColors[1];

uniform vec3 u_camPos;

const float PI = 3.14159265359;

float map(float value, float min1, float max1, float min2, float max2);

float map(float value, float min1, float max1, float min2, float max2)
{
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{

	float waterMask = map(dist, 1, 1.01, 0, 0.5);
	float mappedDist = map(dist, 1, 1.2, 0, 1 );

	vec3 waterColor = vec3(0.04, 0.35, 0.76);
	vec3 landGrass = vec3(0.17255, 0.46275, 0.10196);
	vec3 landBrown = vec3(0.33725, 0.29804, 0.14118);
	float landMixValue = map(mappedDist, 0.05, 1, 0, 1);
	vec3 landColor = mix(landGrass, landBrown, min(landMixValue,1));

	float landWaterMixValue = mappedDist;
	if (landWaterMixValue != 0 && landWaterMixValue <= 0.05)
	{
		landWaterMixValue = 0.8;
	}
	else 
	{
		landWaterMixValue = ceil(mappedDist);
	}

	vec4 overAllColor = vec4(mix(waterColor, landColor, min(waterMask,1)), 1.0);

	float grey = 0.21 * vCol.r + 0.71 * vCol.g + 0.07 * vCol.b;
	vec4 greyScale = vec4(vCol.rgb * (1.0 - 1) + (grey * 1), 1.0);

	//fragColor = overAllColor*((1-greyScale*0.7) + (greyScale*2));
	vec3 albedo = overAllColor.rgb * u_albedo;

	float metallic = (1 - waterMask * 0.8)  * u_metallic;
	float roughness = clamp((waterMask+0.5),0.0,1.0) * u_roughness;
	float ao = u_ao;

	vec3 N = normalize(Normal);
	vec3 V = normalize(u_camPos - WorldPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 1; ++i)//NOTE:Change when adding light
	{
		// calculate per-light radiance
		vec3 L = normalize(u_lightPositions[i] - WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(u_lightPositions[i] - WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = u_lightColors[i] * attenuation;

		// cook-torrance brdf
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);

		// add to outgoing radiance Lo
		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	//fragColor = vec4(temp/);
	fragColor = vec4(color,1.0);
};

