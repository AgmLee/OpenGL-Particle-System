#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float lifetime;
layout(location = 3) in float lifespan;

out vec3 vPosition;
out vec3 vVelocity;
out float vLifetime;
out float vLifespan;
out float vRotation;

uniform float time;
uniform float deltaTime;
uniform float lifeMin;
uniform float lifeMax;
uniform float gravity;
uniform bool useGravity;
uniform vec3 velMin;
uniform vec3 velMax;
uniform vec3 emitterPosition;

const float INVERSE_MAX_UINT = 1.0f / 4294967265.0f;

float Random(uint seed, float range)
{
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^ (i >> 12u);
	return float(range * i) * INVERSE_MAX_UINT;
}

void main()
{
	vPosition = position + velocity * deltaTime;
	vVelocity = velocity;
	if (useGravity)
	{
		vVelocity.y = velocity.y - gravity * deltaTime;
	}
	vLifetime = lifetime + deltaTime;
	vLifespan = lifespan;
	
	if (vLifetime > vLifespan)
	{
		uint seed = uint(time * 1000.0) + uint(gl_VertexID);
		
		vVelocity.x = Random(seed++, velMax.x - velMin.x) + velMin.x;
		vVelocity.y = Random(seed++, velMax.y - velMin.y) + velMin.y;
		vVelocity.z = Random(seed++, velMax.z - velMin.z) + velMin.z;
		
		vPosition = emitterPosition;

		vLifetime = 0;
		vLifespan = Random(seed++, lifeMax - lifeMin) + lifeMin;
	}
}