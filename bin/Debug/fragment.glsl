#version 440 core

out vec4 FragColor;

uniform bool isQuad;
uniform float Level;
uniform vec3 Normal;
uniform vec3 Position;

in vec4 fragPos;

vec3 light = vec3(0.0, 40.0, 10.0);

void main()
{
	vec3 viewDir = normalize(Position - fragPos.rgb);

	vec3 lightDir = normalize(light - fragPos.rgb);
	vec3 reflectDir = reflect(-lightDir, Normal);

	float diff = max(dot(lightDir, Normal), 0.0);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);

	vec3 ambient = vec3(0.1);

	FragColor = vec4(vec3(gl_FragCoord.z)*0.009 , 1.0);
}