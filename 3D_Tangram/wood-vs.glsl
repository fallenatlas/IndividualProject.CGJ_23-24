#version 330 core

in vec3 inPosition;
in vec3 inTexcoord;
in vec3 inNormal;

out vec3 exPosition;
out vec3 exTexcoord;
out vec3 exNormal;

uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	exPosition = inPosition;
	exTexcoord = vec3(inPosition.x * 0.99 * 0.5 + 0.5, (inPosition.z) * 0.99 * 0.5 + 0.5, inPosition.y * 0.99 * 0.5 + 0.5);
	exNormal = vec3(NormalMatrix * vec4(inNormal, 1.0f));

	vec4 MCPosition = vec4(inPosition, 1.0);
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * MCPosition;
}
