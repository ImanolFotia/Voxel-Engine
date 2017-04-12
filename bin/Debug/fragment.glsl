#version 440 core

out vec4 FragColor;

uniform bool isQuad;
uniform float Level;

void main()
{
	FragColor = vec4(1.0, 0.0, 0.0, 1.0);

	if(isQuad)
		FragColor = normalize(vec4(0.0,(Level+1)/12, 1.0, 1.0));
}