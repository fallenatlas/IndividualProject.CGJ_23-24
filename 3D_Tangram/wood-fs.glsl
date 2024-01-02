#version 330 core

uniform sampler3D Texture;

in vec3 exPosition;
in vec3 exTexcoord;
in vec3 exNormal;
in vec3 exFragPositionVC;

vec3 PrimaryColor1 = vec3(0.67451f, 0.48627f, 0.40000f);
vec3 SecondaryColor1 = vec3(0.17255f, 0.14510f, 0.14510f);

//vec3 LightPosition = vec3(2.2f, 3.0f, 3.0f);
vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);

float ambientStrength = 0.5f;
float diffuseStrength = 1.0f;
float specularStrength = 0.5f;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

uniform Light {
   vec3 LightPosition;
};

out vec4 FragmentColor;

void main(void)
{
	vec3 N = normalize(exNormal);
	vec4 lightPositionVC = ViewMatrix * vec4(LightPosition, 1.0);
	vec3 lightDirectionVC = normalize(lightPositionVC.xyz - exFragPositionVC);

	float diff = max(dot(N, lightDirectionVC), 0.0);
	vec3 diffuse = diffuseStrength * diff * LightColor;

	vec3 viewDirectionVC = normalize(-exFragPositionVC);
	vec3 H = normalize(lightDirectionVC + viewDirectionVC);
	//vec3 reflectDirectionVC = reflect(-lightDirectionVC, N); // phong lighting

	//float spec = pow(max(dot(viewDirectionVC, reflectDirectionVC), 0.0), 8); // phong lighting
	float spec = pow(max(dot(N, H), 0.0), 8);
	vec3 specular = specularStrength * spec * LightColor;

	vec3 ambient = ambientStrength * LightColor;

	float lightDistance = length(lightPositionVC.xyz - exFragPositionVC);
	float attenuation = 1 / (1.0f + (0.09f * lightDistance) + (0.032 * (lightDistance * lightDistance)));
	vec3 resultingLight = ambient + (attenuation * (diffuse + specular));

	float k = texture(Texture, exTexcoord).x;
	vec3 resultingColor = resultingLight * mix(SecondaryColor1, PrimaryColor1, k);
	FragmentColor = vec4(resultingColor, 1.0f);
}
