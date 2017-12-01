#version 410

in vec4 gColour;
in vec2 gTexCoords;

out vec4 fragColour;

uniform sampler2D diffuse;
uniform bool maskSet = false;

void main()
{
	vec4 colour = gColour;
	if (colour.w == 0)
	{
		discard;
	}

	if (maskSet)
	{
		vec4 mask = texture(diffuse, gTexCoords);
		if (mask.w == 0)
		{
			discard;
		}
		else
		{
			colour = mask * gColour;
		}
	}
	fragColour = colour;
}