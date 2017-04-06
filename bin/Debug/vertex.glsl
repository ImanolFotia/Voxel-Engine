#version 440 core

layout (location=0) in vec3 positions;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(positions, 1.0);
}