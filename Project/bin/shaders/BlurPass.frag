#version 410

in vec2 vTexCoord;

out vec4 fragColour;

uniform sampler2D target;
uniform int renderMode = 0;

vec4 GaussianBlur()
{
	vec2 gaussFilter[7];
	gaussFilter[0] = vec2(-3.0, 0.00598);
	gaussFilter[1] = vec2(-2.0,	0.060626);
	gaussFilter[2] = vec2(-1.0,	0.241843);
	gaussFilter[3] = vec2(0.0,	0.383103);
	gaussFilter[4] = vec2(1.0,	0.241843);
	gaussFilter[5] = vec2(2.0,	0.060626);
	gaussFilter[6] = vec2(3.0,	0.00598);
	
	vec4 colour = texture(target, vTexCoord);
	for	(int i = 0; i < 7; i++)
	{
		colour += texture(target, vTexCoord + vec2(gaussFilter[i].x, 0)) * gaussFilter[i].y;
	}
	
	return colour;
}
vec4 BoxBlur()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	vec4 colour = texture(target, vTexCoord);
	colour += texture(target, vTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, vTexCoord + vec2(-texel.x, 0));
	colour += texture(target, vTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, vTexCoord + vec2(0, texel.y));
	colour += texture(target, vTexCoord + vec2(0, -texel.y));
	colour += texture(target, vTexCoord + vec2(texel.x, texel.y));
	colour += texture(target, vTexCoord + vec2(texel.x, 0));

	return colour / 9;
}

void main()
{
	if (renderMode == 1)	
		fragColour = BoxBlur();
	else if (renderMode == 2)
		fragColour = GaussianBlur();
	else
		fragColour = texture(target, vTexCoord);
}