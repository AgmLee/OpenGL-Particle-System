#version 410

struct Light
{
	vec4 position;
	vec4 colour;
};

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out VS_OUT
{
	//Vertex Informaiton
	vec3 position;
	vec3 normal;
	vec3 bitangent;
	vec2 texCoords;
	vec3 tangent;
	mat3 TBN;
	//Lighting Information
	vec3 tangentPos;
	vec3 tangentCamPos;
	Light tangentLight;
} vs_out;

uniform vec3 camPos;
uniform Light lights;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

void main()
{
	vec4 worldPos = modelMatrix * position;
	gl_Position = projectionMatrix * worldPos;
	
	vs_out.position = position.xyz;
	vs_out.texCoords = texCoords;
	vs_out.normal = normalize(vec3(modelMatrix * vec4(normals, 0)));
	vs_out.tangent = normalize(vec3(modelMatrix * vec4(tangent, 0)));
	vs_out.bitangent = normalize(vec3(modelMatrix * vec4(bitangent, 0)));
	vs_out.TBN = mat3(vs_out.tangent, vs_out.bitangent, vs_out.normal);
	vec3 tanLightPos = vs_out.TBN * lights.position.xyz;
	vs_out.tangentLight.position = vec4(tanLightPos, lights.position.w);
	vs_out.tangentLight.colour = lights.colour;
	vs_out.tangentCamPos = vs_out.TBN * camPos.xyz;
	vs_out.tangentPos = vs_out.TBN * worldPos.xyz;
}