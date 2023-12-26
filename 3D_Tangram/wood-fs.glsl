#version 330 core

uniform sampler3D Texture;

in vec3 exPosition;
in vec3 exTexcoord;
in vec3 exNormal;

vec4 PrimaryColor1 = vec4(0.67451f, 0.48627f, 0.40000f, 1.0f);
vec4 SecondaryColor1 = vec4(0.17255f, 0.14510f, 0.14510f, 1.0f);

out vec4 FragmentColor;

void main(void)
{
	vec3 N = normalize(exNormal);
	vec3 direction = normalize(vec3(1.0, 0.5, 0.25));
	float intensity = max(dot(direction, N), 0.0) + 0.2f;
	//FragmentColor = Color * vec4(vec3(intensity), 1.0f);
	//FragmentColor = vec4(vec3(exTexcoord, 0.0f), 1.0f);
	//FragmentColor = texture(Texture, exTexcoord) * vec4(vec3(intensity), 1.0f);
	//float brightness = texture(Texture, exTexcoord).x;
	//vec3 gray = vec3(brightness);
	//FragmentColor = vec4(texture(Texture, exTexcoord).x, texture(Texture, exTexcoord).x, texture(Texture, exTexcoord).x, 1.0f);
	//FragmentColor = texture(Texture, exPosition);
	float k = texture(Texture, exTexcoord).x;
	FragmentColor = mix(SecondaryColor1, PrimaryColor1, k);
}
