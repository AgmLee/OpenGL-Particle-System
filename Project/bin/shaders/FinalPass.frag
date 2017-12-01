#version 410

in vec2 vTexCoord;

out vec4 fragColour;

uniform sampler2D fullimage;
uniform sampler2D blurimage;
uniform int renderMode = 0;

vec3 BlurBloom()
{
	vec2 gaussFilter[7];
	gaussFilter[0] = vec2(-3.0, 0.00598);
	gaussFilter[1] = vec2(-2.0,	0.060626);
	gaussFilter[2] = vec2(-1.0,	0.241843);
	gaussFilter[3] = vec2(0.0,	0.383103);
	gaussFilter[4] = vec2(1.0,	0.241843);
	gaussFilter[5] = vec2(2.0,	0.060626);
	gaussFilter[6] = vec2(3.0,	0.00598);
	
	vec4 colour = vec4(0);
	for	(int i = 0; i < 7; i++)
	{
		colour += texture(blurimage, vTexCoord + vec2(0, gaussFilter[i].x)) * gaussFilter[i].y;
	}
	
	return colour.rgb;
}

void main()
{
	vec3 bloom = texture(blurimage, vTexCoord).rgb;
	vec3 full = texture(fullimage, vTexCoord).rgb;

	if (renderMode == 1)
		fragColour = vec4(full, 1);
	else if (renderMode == 2)
		fragColour = vec4(bloom, 1);
	else if (renderMode == 3)
	{
		bloom = BlurBloom();
		fragColour = vec4(full * bloom, 1);
	}
	else
		fragColour = vec4(full * bloom, 1);
}