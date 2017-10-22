#include "Constants.fxh"

float4x4 World;
float4x4 WorldInverseTranspose;
float4x4 WorldViewProj;
float4x4 DirLight0WorldViewProj;
texture DirLight0ShadowMap;
sampler DirLight0ShadowMapSampler = sampler_state
{
	texture = <DirLight0ShadowMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = clamp;
	AddressV = clamp;
};

uint PointLightCount;
float3 PointLightPositions[POINT_MAX_LIGHTS_PER_OBJECT];
float PointLightAttenuations[POINT_MAX_LIGHTS_PER_OBJECT];
textureCUBE Point0ShadowMap;
samplerCUBE Point0ShadowMapSampler = sampler_state
{
	texture = <Point0ShadowMap>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
};

float3 DirLight0Direction;

float4 BoundingFrustum[4];
float4 CustomClippingPlane;

float4x3 Bones[SKINNED_EFFECT_MAX_BONES];

struct VertexShaderInput
{
    float4 Position : POSITION0;
	float3 Normal : NORMAL;
};

struct VertexShaderInputSkinned
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL;
	int4 Indices : BLENDINDICES0;
	float4 Weights : BLENDWEIGHT0;
};

struct VertexShaderOutput
{
	float4 Position : POSITION0;
	float4 PositionWS : TEXCOORD0;
	float4 ClipPlanes : TEXCOORD1;
	float4 PositionDLS : TEXCOORD2;
	float3 Normal : TEXCOORD3;
	float CustomClipPlane : TEXCOORD4;
};

inline void Skin(inout VertexShaderInputSkinned input)
{
	float4x3 skinning = 0;

	[unroll]
	for (int i = 0; i < WEIGHTS_PER_VERTEX; ++i)
	{
		skinning += Bones[input.Indices[i]] * input.Weights[i];
	}

	input.Position.xyz = mul(input.Position, skinning);
}

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

	output.Position = mul(input.Position, WorldViewProj);
	output.PositionWS = mul(input.Position, World);

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

	output.Normal = mul(input.Normal, WorldInverseTranspose);

	output.PositionDLS = mul(input.Position, DirLight0WorldViewProj);

    return output;
}

VertexShaderOutput VertexShaderFunctionSkinned(VertexShaderInputSkinned input)
{
	VertexShaderOutput output;

	Skin(input);

	output.Position = mul(input.Position, WorldViewProj);
	output.PositionWS = mul(input.Position, World);

	output.ClipPlanes.x = dot(output.PositionWS, BoundingFrustum[0]);
	output.ClipPlanes.y = dot(output.PositionWS, BoundingFrustum[1]);
	output.ClipPlanes.z = dot(output.PositionWS, BoundingFrustum[2]);
	output.ClipPlanes.w = dot(output.PositionWS, BoundingFrustum[3]);
	output.CustomClipPlane = dot(output.PositionWS, CustomClippingPlane);

	output.Normal = mul(input.Normal, WorldInverseTranspose);

	output.PositionDLS = mul(input.Position, DirLight0WorldViewProj);

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
	
	float2 projectedDLScoords;
	projectedDLScoords.x = (input.PositionDLS.x / input.PositionDLS.w) / 2.0f + 0.5f;
	projectedDLScoords.y = (-input.PositionDLS.y / input.PositionDLS.w) / 2.0f + 0.5f;
	float depth = tex2D(DirLight0ShadowMapSampler, projectedDLScoords).r;
	float dist = input.PositionDLS.z / input.PositionDLS.w;

	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[branch]
	if ((saturate(projectedDLScoords.x) == projectedDLScoords.x) && (saturate(projectedDLScoords.y) == projectedDLScoords.y))
	{
		[branch]
		if ((dist - SHADOW_BIAS) <= depth || depth <= SHADOW_DEPTH_BIAS)
		{
			color.r = 1.0f;
		}
		else
		{
			float d = dot(normalize(input.Normal), normalize(-DirLight0Direction));
			[branch]
			if (d <= 0)
			{
				color.r = 1.0f;
			}
		}
	}
	else
	{
		color.r = 1.0f;
	}


	
	// point lightz

	[branch]
	if (PointLightCount < 1) return color;

	float3 L;
	float Llength;
	float att;

	L = PointLightPositions[0] - input.PositionWS;
	L.z = -L.z;
	Llength = length(L);
	att = saturate(ATTENUATION_MULTIPLIER * PointLightAttenuations[0] / max(Llength * Llength, MINIMUM_LENGTH_VALUE));

	float shadowMapDepth = texCUBE(Point0ShadowMapSampler, normalize(-(L * att))).r;

	[branch]
	if ((Llength / SHADOW_POINT_MAX_DIST - SHADOW_BIAS) <= shadowMapDepth)
	{
		color.g = 1.0f;
	}
	else
	{
		float d = dot(normalize(input.Normal), normalize(PointLightPositions[0] - input.PositionWS));
		[branch]
		if (d <= 0)
		{
			color.g = 1.0f;
		}
	}

	return color;
}

technique Main
{
	pass Shadow
	{
		VertexShader = compile vs_3_0 VertexShaderFunction();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
};

technique Skinned
{
	pass Shadow
	{
		VertexShader = compile vs_3_0 VertexShaderFunctionSkinned();
		PixelShader = compile ps_3_0 PixelShaderFunction();
	}
};
