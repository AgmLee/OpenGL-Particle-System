#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float lifetime;
layout(location = 3) in float lifespan;
layout(location = 4) in float rotation;

out vec3 vPosition;
out float vLifetime;
out float vLifespan;
out float vRotation;

void main()
{
	vPosition = position;
	vLifetime = lifetime;
	vLifespan = lifespan;
	vRotation = rotation;
}