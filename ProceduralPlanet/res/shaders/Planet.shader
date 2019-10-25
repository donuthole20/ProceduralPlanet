#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 vCol;
out float dist;
out vec3 WorldPos;
out vec3 Normal;

out float biomePercent;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;


uniform int u_biomesCount;

float InverseLerp(float value, float min, float max);

float InverseLerp(float value, float min, float max) 
{
	return ((value - min)) / (max - min);
}

float PHI = 1.61803398874989484820459 * 00000.1; // Golden Ratio   
float PI = 3.14159265358979323846264 * 00000.1; // PI
float SQ2 = 1.41421356237309504880169 * 10000.0; // Square Root of Two

float gold_noise(vec2 coordinate, float seed);

float gold_noise(vec2 coordinate, float seed) {
	return fract(tan(distance(coordinate * (seed + PHI), vec2(PHI, PI))) * SQ2);
}

void main()
{

	float heightPercent = (position.y + 1) / 2f;
	float biomeIndex = 0;
	float blendRange = gold_noise(position.xy, position.z) / 2 + 0.001f;
	
	
	for (int i = 0; i < u_biomesCount; i++)
	{
		int multiplier = i;
		if (i > 0)
		{
			multiplier = i + (i - 1);
		}
		float dst = heightPercent - 0.25 * multiplier;
		
		float weight = clamp(InverseLerp(dst,-blendRange, blendRange),0,1);
		biomeIndex *= (1-weight);
		biomeIndex += i* weight;
	}

	biomePercent = biomeIndex / max(1,2);

	//vCol = vec4(clamp(position,0.0f,1.0f),1.0f);
	vCol = vec4(normal,1.0);
	dist = distance(vec3(0.0), position.xyz);
	//dist =	length(position);
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
in float biomePercent;

uniform vec2 u_elevationMinMax;
uniform sampler2D texture1;
uniform float u_debugFloat;


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
	float elevationNormalized = map(dist, u_elevationMinMax.x, u_elevationMinMax.y, 0, 1);
	
	float grey = 0.21 * vCol.r + 0.71 * vCol.g + 0.07 * vCol.b;
	vec4 greyScale = vec4(vCol.rgb * (1.0 - 1) + (grey * 1), 1.0);

	vec2 texCoord = vec2(biomePercent, elevationNormalized);
	vec4 gradient = texture(texture1, texCoord);
	vec3 albedo = gradient.rgb * u_albedo;

	float metallic = (1 - elevationNormalized * 0.8)  * u_metallic;
	float roughness = clamp((elevationNormalized+0.5),0.0,1.0) * u_roughness;
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

