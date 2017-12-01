#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vPosition[];
in float vLifetime[];
in float vLifespan[];

out vec4 gColour;
out vec2 gTexCoords;

uniform mat4 projection;
uniform mat4 camTransform;

uniform bool drawTri;

uniform float sizeStart;
uniform float sizeEnd;

uniform float rotationStart;
uniform float rotationEnd;

uniform vec4 colourStart;
uniform vec4 colourEnd;

void main()
{
	float lifepoint = vLifetime[0] / vLifespan[0];
	gColour = mix(colourStart, colourEnd, lifepoint);
	
	float halfSize = mix(sizeStart, sizeEnd, lifepoint) * 0.5f;
	
	vec3 corners[4];
	corners[0] = vec3(halfSize,	-halfSize, 0);
	corners[1] = vec3(halfSize,	halfSize, 0);
	corners[2] = vec3(-halfSize, -halfSize, 0);
	corners[3] = vec3(-halfSize, halfSize, 0);

	vec3 zAxis = normalize(camTransform[3].xyz - vPosition[0]);
	vec3 xAxis = cross(camTransform[1].xyz, zAxis);
	vec3 yAxis = cross(zAxis, xAxis);
	mat3 billboard = mat3(xAxis, yAxis, zAxis);
	
	float rotation = mix(rotationStart, rotationEnd, lifepoint);

	vec3 rotAxis = zAxis;
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

	vec3 p = (rMat * billboard * corners[0] + vPosition[0]);
	gl_Position = projection * vec4(p, 1);
	gTexCoords = vec2(0, 0);
	EmitVertex();

	p = (rMat * billboard * corners[1] +  vPosition[0]);
	gl_Position = projection * vec4(p, 1);
	gTexCoords = vec2(1, 0);
	EmitVertex();
	
	p = (rMat * billboard * corners[2] +  vPosition[0]);
	gl_Position = projection * vec4(p, 1);
	gTexCoords = vec2(0, 1);
	EmitVertex();
	
	if (!drawTri)
	{
		p = (rMat * billboard * corners[3] +  vPosition[0]);
		gl_Position = projection * vec4(p, 1);
		gTexCoords = vec2(1, 1);
		EmitVertex();
	}
}