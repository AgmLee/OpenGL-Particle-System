#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec4 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vPosition = view * model * Position;
	vNormal = normalize(view * model * vec4(Normal,0)).xyz;
	vTexCoord = TexCoord;

	gl_Position = projection * model * Position;
}