#version 410

struct Light
{
	vec4 position;
	vec4 colour;
};
struct Texture
{
	vec3 colour;
	sampler2D textureMap;
	bool isSet;
};

in VS_OUT
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
} fs_in;

out vec4 fragColor;

uniform float roughness = 0.5f;
uniform float reflectionCoefficient = 0.5f;

uniform float ambientValue = 0.5f;

uniform Texture diffuseMap;
uniform Texture normalMap;
uniform Texture specularMap;

float DiffuseReflectance(float r2, float NdL, float NdE, vec3 E, vec3 norm, vec3 lightDir)
{
	float A = 1.0f - 0.5f * r2 / (r2 + 0.33f);
	float B = 0.45f * r2 / (r2 + 0.09f);

	vec3 lightProjected = normalize(-lightDir - norm * NdL);
	vec3 viewProjected = normalize(E - norm * NdE);
	float CX = max(0.0f, dot(lightProjected, viewProjected));

	float alpha = sin(max(acos(NdE), acos(NdL)));
	float beta = tan(min(acos(NdE), acos(NdL)));
	float DX = alpha * beta;

	return NdL * (A + B * CX * DX);
}
float SpecularReflectance(float r2, float NdL, float NdE, vec3 E, vec3 norm, vec3 lightDir)
{
	vec3 H = normalize(-lightDir + E);
	float NdH = max(0.0f, dot(norm, H));
	float NdH2 = NdH * NdH;
	float e = 2.71828182845904523536028747135f;
	float pi = 3.1415926535897932384626433832f;

	float exponent = -(1 - NdH2) / (NdH2 * r2);
	float D = pow(e, exponent) / (r2 * NdH2 * NdH2);

	float F = reflectionCoefficient + (1 - reflectionCoefficient) * pow(1 - NdE, 5);

	float X = 2.0f * NdH / dot(E, H);
	float G = min(1, min(X * NdL, X * NdE));

	return max((D*F*G) / (NdE * pi), 0.0f);
}
float Clamp(float value, float cMax, float cMin)
{
	if (value >= cMax)
	{
		return cMax;
	}
	else if (value <= cMin)
	{
		return cMin;
	}
	else
	{
		return value;
	}
}
vec3 CalcBumpedNormal()
{
	if (normalMap.isSet)
	{
		vec3 BumpMapNormal = texture(normalMap.textureMap, fs_in.texCoords).xyz;
		BumpMapNormal = normalize(BumpMapNormal * 2.0 - 1.0);
		return normalize(fs_in.TBN * BumpMapNormal);
	}
	else
	{
		return fs_in.normal;
	}
}

void main()
{
	vec3 lightDir = vec3(0);
	if(fs_in.tangentLight.position.w != 0.0)
	{
		lightDir = normalize(vec3(fs_in.tangentLight.position.xyz) - fs_in.tangentPos);
	}
	else
	{
		lightDir = vec3(fs_in.tangentLight.position.xyz);
	}

	vec3 norm = CalcBumpedNormal();

	vec3 E = normalize(fs_in.tangentCamPos - fs_in.tangentPos);

	float NdL = max(0.0f, dot(norm, -lightDir));
	float NdE = max(0.0f, dot(norm, E));

	float r2 = roughness * roughness;

	float d = DiffuseReflectance(r2, NdL, NdE, E, norm, lightDir);

	float s = SpecularReflectance(r2, NdL, NdE, E, norm, lightDir);

	vec3 diffuse = vec3(1);
	if (diffuseMap.isSet)
	{
		diffuse = texture(diffuseMap.textureMap, fs_in.texCoords).xyz;
	}
	diffuse = diffuse * diffuseMap.colour;

	vec3 specular = vec3(1);
	if (specularMap.isSet)
	{
		specular = texture(specularMap.textureMap, fs_in.texCoords).rgb;
	}
	specular = specular * specularMap.colour;

	vec3 ambient = diffuse * ambientValue;

	fragColor = vec4(ambient + diffuse * d + specular * s, 1);
	//fragColor.rgb = norm;
}
