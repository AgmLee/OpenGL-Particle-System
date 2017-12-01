#version 410

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

layout(location = 0) out vec3 gpassAlbedo;
layout(location = 1) out vec3 gpassPosition;
layout(location = 2) out vec3 gpassNormal;

struct Texture
{
	sampler2D map;
	vec3 colour;
	bool isSet;
};

uniform Texture albedoIn;

vec3 GetColour()
{
	vec3 col = vec3(1);
	if (albedoIn.isSet)
	{
		col = texture(albedoIn.map, vTexCoord).rgb;
	}
	col = col * albedoIn.colour;
	return col;
}

void main()
{
	gpassAlbedo = vNormal;
	gpassPosition = vPosition.xyz;
	gpassNormal = vNormal;
}