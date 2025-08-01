#version 330 core

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec3 aTexCoords;
layout(location = 2) in vec3 vNor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projectedLightSpaceMatrix;

out vec2 TexCoords;
out vec3 nor;
out vec3 FragPosWorldSpace;
out vec4 FragPosProjectedLightSpace;


void main()
{
	gl_Position = projection * view * model * vPos;
	TexCoords = aTexCoords.xy;
	nor = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model*vPos);
    FragPosProjectedLightSpace = projectedLightSpaceMatrix * model * vPos;
	
}
