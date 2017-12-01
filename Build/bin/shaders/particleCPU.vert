#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Colour;
layout(location = 2) in vec2 UV;
layout(location = 3) in vec4 Rotation;

out vec4 vColour;
out vec2 vUV;

uniform mat4 projectionView;

void main()
{
	vColour = Colour;
	vUV = UV;
	gl_Position = projectionView * Position;
}