#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

in vec3 vPosition[];
in float vLifetime[];
in float vLifespan[];

out vec4 gColour;
out vec2 gTexCoords;

uniform mat4 projection;
uniform mat4 camTransform;

uniform float sizeStart;
uniform float sizeEnd;

uniform float rotationStart;
uniform float rotationEnd;
uniform vec3 rotAxis;

uniform vec4 colourStart;
uniform vec4 colourEnd;

void main()
{
	float lifepoint = vLifetime[0] / vLifespan[0];
	gColour = mix(colourStart, colourEnd, lifepoint);
	
	float halfSize = mix(sizeStart, sizeEnd, lifepoint) * 0.5f;
	
	//Setup rotation Matrix
	float rotation = mix(rotationStart, rotationEnd, lifepoint);

	mat3 rMat = mat3 (
		rotAxis.x*rotAxis.x * (1.0f - cos(rotation)) + cos(rotation),
		rotAxis.x*rotAxis.y * (1.0f - cos(rotation)) + rotAxis.z * sin(rotation),
		rotAxis.x*rotAxis.z * (1.0f - cos(rotation)) - rotAxis.y * sin(rotation),
		
		rotAxis.y*rotAxis.x * (1.0f - cos(rotation)) - rotAxis.z * sin(rotation),
		rotAxis.y*rotAxis.y * (1.0f - cos(rotation)) + cos(rotation),
		rotAxis.y*rotAxis.z * (1.0f - cos(rotation)) + rotAxis.x * sin(rotation),
		
		rotAxis.z*rotAxis.x * (1.0f - cos(rotation)) + rotAxis.y * sin(rotation),
		rotAxis.z*rotAxis.y * (1.0f - cos(rotation)) - rotAxis.x * sin(rotation),
		rotAxis.z*rotAxis.z * (1.0f - cos(rotation)) + cos(rotation));

	//Set face uv;
	vec2 uv[4];

	uv[0] = vec2(0, 0);
	uv[1] = vec2(1, 0);
	uv[2] = vec2(0, 1);
	uv[3] = vec2(1, 1);
	
	//Set vertices
	vec3 vertex[24];

	vertex[0] = vec3(halfSize,	-halfSize,	halfSize);
	vertex[1] = vec3(halfSize,	halfSize,	halfSize);
	vertex[2] = vec3(-halfSize,	-halfSize,	halfSize);
	vertex[3] = vec3(-halfSize,	halfSize,	halfSize);

	vertex[4] = vec3(-halfSize,	-halfSize,	-halfSize);
	vertex[5] = vec3(-halfSize,	halfSize,	-halfSize);
	vertex[6] = vec3(halfSize,	-halfSize,	-halfSize);
	vertex[7] = vec3(halfSize,	halfSize,	-halfSize);
	
	vertex[8] = vec3(halfSize,	-halfSize,	-halfSize);
	vertex[9] = vec3(halfSize,	halfSize,	-halfSize);
	vertex[10] = vec3(halfSize,	-halfSize,	halfSize);
	vertex[11] = vec3(halfSize,	halfSize,	halfSize);
	
	vertex[12] = vec3(-halfSize,	-halfSize,	halfSize);
	vertex[13] = vec3(-halfSize,	halfSize,	halfSize);
	vertex[14] = vec3(-halfSize,	-halfSize,	-halfSize);
	vertex[15] = vec3(-halfSize,	halfSize,	-halfSize);
	
	vertex[16] = vec3(halfSize,	halfSize,	halfSize);
	vertex[17] = vec3(halfSize,	halfSize,	-halfSize);
	vertex[18] = vec3(-halfSize,	halfSize,	halfSize);
	vertex[19] = vec3(-halfSize,	halfSize,	-halfSize);
	
	vertex[20] = vec3(halfSize,	-halfSize,	halfSize);
	vertex[21] = vec3(-halfSize,	-halfSize,	halfSize);
	vertex[22] = vec3(halfSize,	-halfSize,	-halfSize);
	vertex[23] = vec3(-halfSize,	-halfSize,	-halfSize);
	
	//Construct faces
	for (uint i = 0; i < 24; i += 4)
	{
		gl_Position = projection * vec4(rMat * vertex[i + 0] + vPosition[0], 1);
		gTexCoords = uv[0];
		EmitVertex();
		
		gl_Position = projection * vec4(rMat * vertex[i + 1] + vPosition[0], 1);
		gTexCoords = uv[1];
		EmitVertex();
		
		gl_Position = projection * vec4(rMat * vertex[i + 2] + vPosition[0], 1);
		gTexCoords = uv[2];
		EmitVertex();
		
		gl_Position = projection * vec4(rMat * vertex[i + 3] + vPosition[0], 1);
		gTexCoords = uv[3];
		EmitVertex();
		EndPrimitive();
	}
}