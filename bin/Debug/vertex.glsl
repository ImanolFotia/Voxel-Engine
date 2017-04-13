#version 440 core

layout (location=0) in vec3 positions;

uniform mat4 MVP;
uniform mat4 model;

out vec4 fragPos;

void main()
{
	fragPos = model * vec4(positions, 1.0);

	gl_Position = MVP * vec4(positions, 1.0);

}