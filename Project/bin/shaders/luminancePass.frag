#version 410

in vec2 vTexCoord;

out vec4 fragColour;

uniform sampler2D target;
uniform float exposure = 1.0f;
uniform float luminCheck = 1.0f;
uniform int renderMode = 0;

vec4 ColourPass(vec3 colour)
{
	colour *= exposure;

	vec3 x;
	x.r = max(0, colour.r - 0.004f);
	x.g = max(0, colour.g - 0.004f);
	x.b = max(0, colour.b - 0.004f);

	colour = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);

	return vec4(colour, 1);
}

void main()
{
	vec4 colour = texture(target, vTexCoord);
	float luminance = dot(colour.rgb, vec3(0.2125f, 0.7154f, 0.0721f));

	if (renderMode == 1)
		fragColour = vec4(luminance, luminance, luminance, 1);
	else if (renderMode == 2)
		fragColour = vec4(colour.rgb * luminance, colour.a);
	else
		fragColour = colour;
}