#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec4 vCol;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


void main()
{
	gl_Position = projectionMatrix *  modelMatrix * viewMatrix * vec4(position,1.0f);
	//vCol = vec4(clamp(position,0.0f,1.0f),1.0f);
	vCol = vec4(position,1.0);
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 vCol;

float map(float value, float min1, float max1, float min2, float max2);

float map(float value, float min1, float max1, float min2, float max2)
{
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}


void main()
{

	float dist = distance(vec3(0.0), vCol.xyz);
	float waterMask = map(dist, 1, 1.01, 0, 0.5);
	float mappedDist = map(dist, 1, 1.2, 0, 1 );

	vec3 waterColor = vec3(0.12549, 0.27843, 0.36471);
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

	vec3 overAllColor = mix(waterColor, landColor, min(waterMask,1));

	color = vec4(overAllColor,1.0);
};

