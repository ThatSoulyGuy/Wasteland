#version 410 core

layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec3 colorIn;
layout (location = 2) in vec3 normalIn;
layout (location = 3) in vec2 uvsIn;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 color;
out vec3 normal;
out vec2 uvs;

void main()
{
	gl_Position = vec4(positionIn, 1.0)  * model * view * projection;

	color = colorIn;
	normal = normalIn;
	uvs = uvsIn;
}