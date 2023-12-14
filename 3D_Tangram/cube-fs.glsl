#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

uniform vec4 Color;

out vec4 FragmentColor;

void main(void)
{
	vec3 N = normalize(exNormal);
	vec3 direction = normalize(vec3(1.0, 0.5, 0.25));
	float intensity = max(dot(direction, N), 0.0) + 0.2f;
	FragmentColor = Color * vec4(vec3(intensity), 1.0f);
}
