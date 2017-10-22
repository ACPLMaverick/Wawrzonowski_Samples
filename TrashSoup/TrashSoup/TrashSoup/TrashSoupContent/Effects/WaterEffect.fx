#include "Constants.fxh"

float4x4 World;
float4x4 WorldViewProj;
float4x4 WorldInverseTranspose;
float4x4 ReflectViewProj;

float3 AmbientLightColor;

float3 DirLight0Direction;
float3 DirLight0DiffuseColor;
float3 DirLight0SpecularColor;
float3 DirLight1Direction;
float3 DirLight1DiffuseColor;
float3 DirLight1SpecularColor;
float3 DirLight2Direction;
float3 DirLight2DiffuseColor;
float3 DirLight2SpecularColor;

float3 PointLightDiffuseColors[POINT_MAX_LIGHTS_PER_OBJECT];
float3 PointLightPositions[POINT_MAX_LIGHTS_PER_OBJECT];
float3 PointLightSpecularColors[POINT_MAX_LIGHTS_PER_OBJECT];
float PointLightAttenuations[POINT_MAX_LIGHTS_PER_OBJECT];
uint PointLightCount;

float4 BoundingFrustum[4];
float4 CustomClippingPlane;

texture DiffuseMap;
sampler DiffuseSampler = sampler_state
{
	texture = <DiffuseMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

texture NormalMap;
sampler NormalSampler = sampler_state
{
	texture = <NormalMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

texture ReflectionMap;
sampler ReflectionSampler = sampler_state
{
	texture = <ReflectionMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

texture RefractionMap;
sampler RefractionSampler = sampler_state
{
	texture = <RefractionMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Mirror;
	AddressV = Mirror;
};

float3 EyePosition;

float2 WindVector;
float DeltaTime;

float3 DiffuseColor;
float3 SpecularColor;
float Glossiness;
float3 ReflectivityColor;
float ReflectivityBias;
float Transparency;
float3 padding01;

struct VertexShaderInput
{
    float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct VertexShaderOutput
{
	float4 Position : POSITION0;
	float4 PositionWS : TEXCOORD2;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float4 ClipPlanes : TEXCOORD3;
	float CustomClipPlane : TEXCOORD4;
	float4 ReflectionMapCoord : TEXCOORD5;
	float4 RefractionMapCoord : TEXCOORD6;
};

struct ColorPair
{
	float3 Diffuse;
	float3 Specular;
};

inline void ComputeSingleLight(float3 L, float3 color, float3 specularColor, float3 E, float3 N, inout ColorPair pair)
{
	float3 H = normalize(normalize(E) + L);
	float intensity = max(dot(L, N), 0.0f);
	float3 specular = pow(max(0.0000001f, dot(H, N)), Glossiness) * length(specularColor);

	pair.Diffuse += intensity * color;
	pair.Specular += specular * specularColor * pair.Diffuse;
}

ColorPair ComputeLight(float3 posWS, float3 E, float3 N)
{
	E = normalize(E);
	N = normalize(N);

	ColorPair result;
	ColorPair temp;

	result.Diffuse = AmbientLightColor;
	result.Specular = 0;
	temp.Diffuse = 0;
	temp.Specular = 0;

	// DirLight0
	ComputeSingleLight(-DirLight0Direction, DirLight0DiffuseColor,
		float3(DirLight0SpecularColor.x * SpecularColor.x, DirLight0SpecularColor.y * SpecularColor.y, DirLight0SpecularColor.z * SpecularColor.z), E, N, result);

	// DirLight1
	ComputeSingleLight(-DirLight1Direction, DirLight1DiffuseColor,
		float3(DirLight1SpecularColor.x * SpecularColor.x, DirLight1SpecularColor.y * SpecularColor.y, DirLight1SpecularColor.z * SpecularColor.z), E, N, result);

	// DirLight2
	ComputeSingleLight(-DirLight2Direction, DirLight2DiffuseColor,
		float3(DirLight2SpecularColor.x * SpecularColor.x, DirLight2SpecularColor.y * SpecularColor.y, DirLight2SpecularColor.z * SpecularColor.z), E, N, result);

	// point lights
	float3 L;
	float Llength;
	float att;
	for (uint i = 0; i < PointLightCount; ++i)
	{
		L = PointLightPositions[i] - posWS;
		Llength = length(L);
		ComputeSingleLight(normalize(L), PointLightDiffuseColors[i],
			float3(PointLightSpecularColors[i].x * SpecularColor.x, PointLightSpecularColors[i].y * SpecularColor.y, PointLightSpecularColors[i].z * SpecularColor.z),
			E, N, temp);

		att = saturate(ATTENUATION_MULTIPLIER * length(PointLightDiffuseColors[i]) * PointLightAttenuations[i] / max(Llength * Llength, MINIMUM_LENGTH_VALUE));
		temp.Diffuse = temp.Diffuse * att;
		temp.Specular = temp.Specular * att;
		result.Diffuse += temp.Diffuse;
		result.Specular += temp.Specular;

		temp.Diffuse = 0;
		temp.Specular = 0;
	}


	return result;
}

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

	output.Position = mul(input.Position, WorldViewProj);

	output.PositionWS = mul(input.Position, World);

	output.TexCoord = input.TexCoord;
	output.TexCoord.x = output.TexCoord.x + WindVector.x;
	output.TexCoord.y = output.TexCoord.y + WindVector.y;

	output.Normal = normalize(mul(input.Normal, WorldInverseTranspose));

	output.ReflectionMapCoord = mul(input.Position, ReflectViewProj);
	output.RefractionMapCoord = mul(input.Position, WorldViewProj);

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	// clippin

	clip(input.ClipPlanes.x);
	clip(input.ClipPlanes.y);
	clip(input.ClipPlanes.z);
	clip(input.ClipPlanes.w);
	clip(input.CustomClipPlane);

	//////

	float4 color = tex2D(DiffuseSampler, input.TexCoord);
	float alpha = color.a;
	color.a = 1.0f;

	// computin normals

	float3 nAdj = (tex2D(NormalSampler, input.TexCoord)).xyz;
	input.Normal = normalize(input.Normal);

	nAdj.x = (nAdj.x * 2) - 1;
	nAdj.y = (nAdj.y * 2) - 1;
	nAdj.z = (nAdj.z * 2) - 1;

	input.Normal = input.Normal + nAdj;
	input.Normal = normalize(input.Normal);

	////////

	// computin water
	
	float3 finalWater = float3(0, 0, 0);
	float fresnel = saturate(dot(normalize(EyePosition - input.PositionWS.xyz), float3(0.0f, 1.0f, 0.0f)));
	float pScale = 0.25f;

	float2 projectedCoordsRF;
	projectedCoordsRF.x = (input.ReflectionMapCoord.x / input.ReflectionMapCoord.w) / 2.0f + 0.5f;
	projectedCoordsRF.y = (-input.ReflectionMapCoord.y / input.ReflectionMapCoord.w) / 2.0f + 0.5f;

	projectedCoordsRF = projectedCoordsRF + (nAdj.xy * pScale);
	float3 refl = tex2D(ReflectionSampler, projectedCoordsRF);

	float2 projectedCoordsRR;
	projectedCoordsRR.x = input.RefractionMapCoord.x / input.RefractionMapCoord.w / 2.0f + 0.5f;
	projectedCoordsRR.y = -input.RefractionMapCoord.y / input.RefractionMapCoord.w / 2.0f + 0.5f;

	projectedCoordsRR = projectedCoordsRR + (nAdj.xy * pScale * 0.25f);
	float3 refr = tex2D(RefractionSampler, projectedCoordsRR);
	
	finalWater = lerp(refl, refr, clamp(fresnel, 0.5f, 1.0f));

	////////

	ColorPair computedLight = ComputeLight(input.PositionWS.xyz, EyePosition - input.PositionWS.xyz, input.Normal);

	float4 startColor = color * float4(computedLight.Diffuse, 1.0f);
	color =  lerp(startColor, alpha * float4(finalWater, 1.0f), ReflectivityBias);
	float3 specular = alpha * computedLight.Specular;
		color = float4(DiffuseColor, 1.0f) * color + float4(specular, 1.0f);

	color *= Transparency;

    return color;
}

technique Main
{
    pass Pass1
    {
        VertexShader = compile vs_3_0 VertexShaderFunction();
        PixelShader = compile ps_3_0 PixelShaderFunction();
    }
}
