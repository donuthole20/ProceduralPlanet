#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 vCol;
out float dist;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


void main()
{
	gl_Position = projectionMatrix *  modelMatrix * viewMatrix * vec4(position,1.0f);
	//vCol = vec4(clamp(position,0.0f,1.0f),1.0f);
	vCol = vec4(normal,1.0);
	dist = distance(vec3(0.0), position.xyz);
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 vCol;
in float dist;

float map(float value, float min1, float max1, float min2, float max2);

float map(float value, float min1, float max1, float min2, float max2)
{
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
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

	color = overAllColor*((1-greyScale*0.7) + (greyScale*2));
};

