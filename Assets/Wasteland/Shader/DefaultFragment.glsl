#version 410 core

out vec4 FragColor;

uniform sampler2D diffuse;

in vec3 color;
in vec3 normal;
in vec2 uvs;

void main()
{
	FragColor = texture(diffuse, uvs) * vec4(color, 1.0);
}