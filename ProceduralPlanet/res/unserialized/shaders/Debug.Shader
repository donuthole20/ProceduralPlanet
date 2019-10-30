#shader vertex
#version 330 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec2 l_uv;

out vec2 uv;

void main()
{
	uv = l_uv;
	gl_Position = vec4(l_position,1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;

in vec2 uv;

uniform sampler2D texture1;

void main()
{
	fragColor = texture(texture1, uv);
}