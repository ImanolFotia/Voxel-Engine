#version 440 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in mat4 positions;

uniform mat4 MVP;
uniform mat4 model;

uniform mat4 view;
uniform mat4 projection;

out vec4 fragPos;
out vec3 Normal;

void main()
{
	fragPos = positions * vec4(position, 1.0);
	Normal = normal;

	gl_Position = projection * view * (positions) * vec4(position, 1.0);

}