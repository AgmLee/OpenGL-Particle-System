#version 410

in vec4 vColour;
in vec2 vUV;
out vec4 fragColour;

uniform sampler2D clippingMask;
//Used instead if the clipping mask isn't set
uniform bool maskIsSet = false;
//Determins if black means alpha or colour
uniform bool useMaskAlphaOnly = false;
//Clipping Colour (Used if useMaskAlphaOnly is false)
uniform vec3 clippingColour = vec3 (0.0f);
uniform float sharpness = 0.1f;

void main()
{
	vec4 mask = vec4(1);
	//Check for alphaMask
	if(maskIsSet)
	{
		//Get Mask values
		mask.rgba = texture(clippingMask, vUV);
		//Check if using only the alpha or the colour
		if (useMaskAlphaOnly)
		{
			//Dim the colour by the masks value
			if (mask.w == 0)
			{
				discard;
			}
		}
		else
		{
			if (distance(mask.rgb, clippingColour) < sharpness)
			{
				discard;
			}
		}
	}
	else if (vColour.w == 0)
	{
		discard;
	}
	fragColour = vec4(vColour.rgb * mask.rgb, vColour.w);
	//fragColour.rgb = mask.rgb;
}